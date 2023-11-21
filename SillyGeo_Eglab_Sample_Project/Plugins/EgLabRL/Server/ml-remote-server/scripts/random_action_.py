import unreal_engine as ue
from mlpluginapi import MLPluginAPI
import numpy as np

class random_action(MLPluginAPI):
    def on_setup(self):
        self.max_motion = 9
        pass


    def on_json_input(self, jsonInput):
        ue.log('jsonInput call')
        observation = jsonInput['observation']
        reward = jsonInput['reward']
        done = jsonInput['done']
        ue.log('observation shape: ' + str(observation))
        ue.log('reward: ' + str(reward))
        ue.log('done: ' + str(done))

        result = {}
        result['move'] = np.random.randint(0, self.max_motion)
        result['rotation'] = np.random.randint(0, self.max_motion)
        result['change_weapon'] = np.random.randint(0, 2)
        ue.log('result : ' + str(result['move']) + ', ' + str(result['rotation']) + ', '  + str(result['change_weapon']))
        return result

    def on_begin_training(self):
        pass


    def on_stop_training(self):
        pass


def get_api():
    return random_action.get_instance()