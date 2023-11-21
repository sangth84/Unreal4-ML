

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function



from absl import app
from absl import flags
from dopamine.agents.dqn import dqn_agent
from dopamine.agents.implicit_quantile import implicit_quantile_agent
from dopamine.agents.rainbow import rainbow_agent
from dopamine.unreal import run_experiment

import tensorflow as tf
import sys
import os
import  numpy as np

agent_name = 'rainbow'
base_dir = os.path.join(os.path.dirname(os.path.realpath(__file__)), '../../Saved/tf')
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
  if agent_name == 'dqn':
    return dqn_agent.DQNAgent(sess, num_actions=environment.action_space,
                              summary_writer=summary_writer,
                              observation_shape=environment.observation_space)
  elif agent_name == 'rainbow':
    return rainbow_agent.RainbowAgent(
        sess, num_actions=environment.action_space,
        summary_writer=summary_writer,
        observation_shape=environment.observation_space)
  elif agent_name == 'implicit_quantile':
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

  assert base_dir is not None
  return run_experiment.Runner(base_dir, create_agent_fn)


class train():
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

      self.runner = create_runner(base_dir, create_agent)

  def reset_game(self):
      self.current_frame = 0
      self.accumulated_reward = 0
      self.result = {'move':8, 'rotation':8, 'change_weapon':0}

  def onSetup(self):
      if self.runner.check_end_iteration():
          self.end_game()

      self.runner.set_evel_mode( False )

  def onJsonInput(self, jsonInput):
      observation = jsonInput['observation']
      reward = jsonInput['reward']
      done = jsonInput['done']

      self.accumulated_reward += reward

      if self.current_frame % self.frame_skip == 0:
        action = self.runner.run_one_step(observation, self.accumulated_reward, done)

        self.accumulated_reward = 0
        self.result['move'] = action[0] // 9
        self.result['rotation'] = action[0] % 9
        self.result['change_weapon'] = 0 # action[2]
      else:
          self.result['change_weapon'] = 0

      if self.runner.check_end_iteration():
          self.end_game()

      if done:
          self.reset_game()

      self.current_frame += 1

      return self.result

  def onBeginTraining(self):
      pass

  def onStopTraining(self):
      pass

  def end_game(self):
    pass

temp = train()
temp.onSetup()
data={}
data['observation'] = np.zeros((204,),dtype=np.float)
data['reward'] = 1.0
data['done'] = False
count = 0
while(True):
    temp.onJsonInput(data)
    if count % 20 == 0:
        data['done'] = True
    else:
        data['done'] = False
    count += 1