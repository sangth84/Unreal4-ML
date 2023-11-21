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
r"""The entry point for running an agent on an Atari 2600 domain.

"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import sys
import os

sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))

from absl import app
from absl import flags
from dopamine.agents.dqn import dqn_agent
from dopamine.agents.implicit_quantile import implicit_quantile_agent
from dopamine.agents.rainbow import rainbow_agent
from dopamine.unreal import run_experiment
from dopamine.unreal import preprocessing
from mlpluginapi import MLPluginAPI

import tensorflow as tf
import unreal_engine as ue
import  numpy as np

agent_name = 'Rainbow'
base_dir = os.path.join(os.path.abspath(os.path.dirname(__file__)), '\\Saved\\')
gin_files = [os.path.join(os.path.dirname(os.path.realpath(__file__)), './dopamine/agents/dqn/configs/dqn.gin')]
gin_bindings = []

def create_agent(sess, environment, summary_writer=None):
  """Creates a DQN agent.

  Args:
    sess: A `tf.Session` object for running associated ops.
    environment: An Atari 2600 Gym environment.
    summary_writer: A Tensorflow summary writer to pass to the agent
      for in-agent training statistics in Tensorboard.

  Returns:
    agent: An RL agent.

  Raises:
    ValueError: If `agent_name` is not in supported list.
  """
  if agent_name == 'Dqn':
    return dqn_agent.DQNAgent(sess, num_actions=environment.action_space,
                              summary_writer=summary_writer,
                              observation_shape=environment.observation_space)
  elif agent_name == 'Rainbow':
    return rainbow_agent.RainbowAgent(
        sess, num_actions=environment.action_space,
        summary_writer=summary_writer,
        observation_shape=environment.observation_space)
  elif agent_name == 'Implicit_quantile':
    return implicit_quantile_agent.ImplicitQuantileAgent(
        sess, num_actions=environment.action_space,
        summary_writer=summary_writer,
        observation_shape=environment.observation_space)
  else:
    raise ValueError('Unknown agent: {}'.format(agent_name))


def create_runner(base_dir, create_agent_fn):
  """Creates an experiment Runner.

  Args:
    base_dir: str, base directory for hosting all subdirectories.
    create_agent_fn: A function that takes as args a Tensorflow session and an
     Atari 2600 Gym environment, and returns an agent.

  Returns:
    runner: A `run_experiment.Runner` like object.

  Raises:
    ValueError: When an unknown schedule is encountered.
  """

  return run_experiment.Runner(base_dir, create_agent_fn)


class train(MLPluginAPI):
  def __init__(self):
      super(train, self).__init__()
      self.runner = None
      self.end_iteration = False
      self.is_train_mode = True
      self.one_step_fn = None
      tf.logging.set_verbosity(tf.logging.INFO)
      self.frame_skip = 4
      self.current_frame = 0
      self.reset_game()
      #self.scripts_path = os.path.abspath(os.path.dirname(__file__)) + "/Saved_" + agent_name
      self.scripts_path = os.path.abspath(os.path.dirname(__file__)) + "\\Saved"
      if not os.path.exists( self.scripts_path ):
        os.makedirs( self.scripts_path )

      self.runner = create_runner(self.scripts_path, create_agent)

  def reset_game(self):
      self.current_frame = 0
      self.accumulated_reward = 0
      self.result = {'move':0, 'rotation':0, 'change_weapon':0}

  def on_setup(self):
      if self.runner.check_end_iteration():
          self.end_game()

      self.runner.set_evel_mode( False )

  def on_json_input(self, jsonInput):
      observation = jsonInput['observation']
      reward = jsonInput['reward']
      done = jsonInput['done']

      self.accumulated_reward += reward

      if self.current_frame % self.frame_skip == 0:
        action = self.runner.run_one_step(observation, self.accumulated_reward, done)

        self.accumulated_reward = 0
        self.result['move'] = int(action[0] // 9)
        self.result['rotation'] = int(action[0] % 9)
        self.result['change_weapon'] = 0
      else:
          self.result['change_weapon'] = 0

      if self.runner.check_end_iteration():
          self.end_game()

      if done:
          self.reset_game()

      self.current_frame += 1

      return self.result

  def on_begin_training(self):
      pass

  def on_stop_training(self):
      pass

  def end_game(self):
    pass

def get_api(observation_space, agentType):
  if observation_space > 0:
    preprocessing.gl_observation_space = (observation_space,)
  global agent_name
  agent_name = agentType
  return train.get_instance()