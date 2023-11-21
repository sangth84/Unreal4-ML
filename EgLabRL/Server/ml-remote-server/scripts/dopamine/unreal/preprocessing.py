
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np
import gin.tf

gl_observation_space = (204,)

@gin.configurable
class stockPreprocessing(object):

  def __init__(self, skip=4):
    self.skip = skip
    self.last_observation = None

  @property
  def observation_space(self):
    return gl_observation_space

  @property
  def action_space(self):
    return (81,)

  def reset(self):
    self.last_observation = None
    return np.zeros(self.observation_space, dtype=np.float32)