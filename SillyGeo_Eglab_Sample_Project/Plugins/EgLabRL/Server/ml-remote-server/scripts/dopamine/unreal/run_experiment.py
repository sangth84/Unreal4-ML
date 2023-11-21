# coding=utf-8
# Copyright 2018 The Dopamine Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Module defining classes and helper methods for running Atari Retro agents."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import sys
import time

from dopamine.unreal import preprocessing
from dopamine.common import checkpointer
from dopamine.common import iteration_statistics
from dopamine.common import logger

import numpy as np
import tensorflow as tf
import gin.tf

def load_gin_configs(gin_files, gin_bindings):
  """Loads gin configuration files.

  Args:
    gin_files: list, of paths to the gin configuration files for this
      experiment.
    gin_bindings: list, of gin parameter bindings to override the values in
      the config files.
  """
  gin.parse_config_files_and_bindings(gin_files,
                                      bindings=gin_bindings,
                                      skip_unknown=False)


def create_atari_environment(codes):
  env = preprocessing.stockPreprocessing(codes)
  return env


@gin.configurable
class Runner(object):
  """Object that handles running Atari Retro experiments.

  Here we use the term 'experiment' to mean simulating interactions between the
  agent and the environment and reporting some statistics pertaining to these
  interactions.

  A simple scenario to train a DQN agent is as follows:

  ```python
  base_dir = '/tmp/simple_example'
  def create_agent(sess, environment):
    return dqn_agent.DQNAgent(sess, num_actions=environment.action_space.n)
  runner = Runner(base_dir, create_agent, game_name='Pong')
  runner.run()
  ```
  """

  def __init__(self,
               base_dir,
               create_agent_fn,
               create_environment_fn=create_atari_environment,
               codes=None,
               checkpoint_file_prefix='ckpt',
               logging_file_prefix='log',
               log_every_n=1,
               num_iterations=200,
               training_steps=5000,
               evaluation_steps=12500,
               max_steps_per_episode=200000):
    """Initialize the Runner object in charge of running a full experiment.

    Args:
      base_dir: str, the base directory to host all required sub-directories.
      create_agent_fn: A function that takes as args a Tensorflow session and an
        Atari Retro Gym environment, and returns an agent.
      create_environment_fn: A function which receives a game name and creates
        an Atari Retro Gym environment.
      game_name: str, name of the Atari Retro domain to run.
      state_name: str, make the initial settings to the saved movie state.
      checkpoint_file_prefix: str, the prefix to use for checkpoint files.
      logging_file_prefix: str, prefix to use for the log files.
      log_every_n: int, the frequency for writing logs.
      num_iterations: int, the iteration number threshold (must be greater than
        start_iteration).
      training_steps: int, the number of training steps to perform.
      evaluation_steps: int, the number of evaluation steps to perform.
      max_steps_per_episode: int, maximum number of steps after which an episode
        terminates.

    This constructor will take the following actions:
    - Initialize an environment.
    - Initialize a `tf.Session`.
    - Initialize a logger.
    - Initialize an agent.
    - Reload from the latest checkpoint, if available, and initialize the
      Checkpointer object.
    """
    assert base_dir is not None
    self._logging_file_prefix = logging_file_prefix
    self._log_every_n = log_every_n
    self._num_iterations = num_iterations
    self._training_steps = training_steps
    self._evaluation_steps = evaluation_steps
    self._max_steps_per_episode = max_steps_per_episode
    self._base_dir = base_dir
    self._create_directories()
    self._summary_writer = tf.summary.FileWriter(self._base_dir)

    self._environment = create_environment_fn(codes)
    # Set up a session and initialize variables.
    self._sess = tf.Session('',
                            config=tf.ConfigProto(allow_soft_placement=True))
    self._agent = create_agent_fn(self._sess, self._environment,
                                  summary_writer=self._summary_writer)
    self._summary_writer.add_graph(graph=tf.get_default_graph())
    self._sess.run(tf.global_variables_initializer())

    self._initialize_checkpointer_and_maybe_resume(checkpoint_file_prefix)

    self._use_eval = False
    self._init_iteration()

  def set_evel_mode(self, value):
    self._use_eval = value
    self._agent.eval_mode = value

  def _init_iteration(self):
    self.statistics = iteration_statistics.IterationStatistics()
    tf.logging.info('Starting iteration %d', self.current_iteration)
    self.start_time = time.time()
    self._init_one_phase()


  def _end_iteration(self):
    self._log_experiment(self.current_iteration, self.statistics)
    self._checkpoint_experiment(self.current_iteration)
    self.current_iteration += 1

  def _init_one_phase(self):
    self.step_count = 0
    self.num_episodes = 0
    self.sum_returns = 0.

    self._init_one_episode()

  def _run_one_phase(self, episode_length, episode_return):
    self.statistics.append({
      'episode_lengths': episode_length,
      'episode_returns': episode_return
    })
    self.step_count += episode_length
    self.sum_returns += episode_return
    self.num_episodes += 1
    # We use sys.stdout.write instead of tf.logging so as to flush frequently
    # without generating a line break.
    sys.stdout.write('Steps executed: {} '.format(self.step_count) +
                     'Episode length: {} '.format(episode_length) +
                     'Return: {}\r'.format(episode_return))
    sys.stdout.flush()

  def _end_one_phase(self):
    average_return = self.sum_returns / self.num_episodes if self.num_episodes > 0 else 0.0
    self.statistics.append({'average_return': average_return})
    time_delta = time.time() - self.start_time
    tf.logging.info('Average undiscounted return per episode: %.2f',
                    average_return)
    tf.logging.info('Average steps per second: %.2f',
                    self.step_count / time_delta)

    self._save_tensorboard_summaries(self.current_iteration, self.num_episodes,
                                     average_return)
    self._end_iteration()
    self._init_iteration()

  def _init_one_episode(self):
    self.total_reward = 0
    self.step_number = 0
    initial_observation = self._environment.reset()
    self._agent.begin_episode(initial_observation)

  def _end_episode(self):
    self._run_one_phase(self.step_number, self.total_reward)
    if self.step_count > self._training_steps:
      self._end_one_phase()
    else:
      self._init_one_episode()

  def _create_directories(self):
    """Create necessary sub-directories."""
    self._checkpoint_dir = os.path.join(self._base_dir, 'checkpoints')
    self._logger = logger.Logger(os.path.join(self._base_dir, 'logs'))

  def _initialize_checkpointer_and_maybe_resume(self, checkpoint_file_prefix):
    """Reloads the latest checkpoint if it exists.

    This method will first create a `Checkpointer` object and then call
    `checkpointer.get_latest_checkpoint_number` to determine if there is a valid
    checkpoint in self._checkpoint_dir, and what the largest file number is.
    If a valid checkpoint file is found, it will load the bundled data from this
    file and will pass it to the agent for it to reload its data.
    If the agent is able to successfully unbundle, this method will verify that
    the unbundled data contains the keys,'logs' and 'current_iteration'. It will
    then load the `Logger`'s data from the bundle, and will return the iteration
    number keyed by 'current_iteration' as one of the return values (along with
    the `Checkpointer` object).

    Args:
      checkpoint_file_prefix: str, the checkpoint file prefix.

    Returns:
      start_iteration: int, the iteration number to start the experiment from.
      experiment_checkpointer: `Checkpointer` object for the experiment.
    """
    self._checkpointer = checkpointer.Checkpointer(self._checkpoint_dir,
                                                   checkpoint_file_prefix)
    self._start_iteration = 0
    self.current_iteration = 0
    # Check if checkpoint exists. Note that the existence of checkpoint 0 means
    # that we have finished iteration 0 (so we will start from iteration 1).
    latest_checkpoint_version = checkpointer.get_latest_checkpoint_number(
        self._checkpoint_dir)
    if latest_checkpoint_version >= 0:
      experiment_data = self._checkpointer.load_checkpoint(
          latest_checkpoint_version)
      if self._agent.unbundle(
          self._checkpoint_dir, latest_checkpoint_version, experiment_data):
        assert 'logs' in experiment_data
        assert 'current_iteration' in experiment_data
        self._logger.data = experiment_data['logs']
        self._start_iteration = experiment_data['current_iteration'] + 1
        self.current_iteration = self._start_iteration
        tf.logging.info('Reloaded checkpoint and will start from iteration %d',
                        self._start_iteration)

  def run_one_step(self, observation, reward, is_terminal):
    self.total_reward += reward
    self.step_number += 1

    # Perform reward clipping.
    # reward = np.clip(reward, -1, 1)

    if is_terminal:
      self._agent.end_episode(reward)
      action = self._agent.begin_episode(observation)
      self._end_episode()
    else:
      action = self._agent.step(reward, observation)

    return action

  def _save_tensorboard_summaries(self, iteration,
                                  num_episodes,
                                  average_reward):
    summary = tf.Summary(value=[
      tf.Summary.Value(tag='NumEpisodes',
                       simple_value=num_episodes),
      tf.Summary.Value(tag='AverageReturns',
                       simple_value=average_reward)
    ])
    self._summary_writer.add_summary(summary, iteration)


  def check_end_iteration(self):
    if self._num_iterations <= self._start_iteration:
      tf.logging.warning('num_iterations (%d) < start_iteration(%d)',
                         self._num_iterations, self._start_iteration)
      return True
    return False

  def _log_experiment(self, iteration, statistics):
    """Records the results of the current iteration.

    Args:
      iteration: int, iteration number.
      statistics: `IterationStatistics` object containing statistics to log.
    """
    self._logger['iteration_{:d}'.format(iteration)] = statistics
    if iteration % self._log_every_n == 0:
      self._logger.log_to_file(self._logging_file_prefix, iteration)

  def _checkpoint_experiment(self, iteration):
    """Checkpoint experiment data.

    Args:
      iteration: int, iteration number for checkpointing.
    """
    experiment_data = self._agent.bundle_and_checkpoint(self._checkpoint_dir,
                                                        iteration)
    if experiment_data:
      experiment_data['current_iteration'] = iteration
      experiment_data['logs'] = self._logger.data
      self._checkpointer.save_checkpoint(iteration, experiment_data)