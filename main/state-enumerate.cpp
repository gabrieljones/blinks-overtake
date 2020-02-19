#include "state-enumerate.h"
#include "animate.h"
#include "game-def.h"
#include "action.h"
#include "global-events.h"
#include "distributed-task.h"

namespace stateEnumerate {
    
    byte _enumeration;
    byte _totalEnumerations;
    bool _error;

    byte enumerateHandler(const byte op, const byte payload) {
        if(op == DISTRIBUTED_TASK_OP_PASSING_IN) {
            _enumeration = payload+1;
            return payload+1;
        }
        if(op == DISTRIBUTED_TASK_OP_PASSED_DONE) {
            _enumeration = _enumeration - 1; //zero index
            _totalEnumerations = payload;
            if(_totalEnumerations < 4) {
                _error = true;
                return payload;
            }
            stateCommon::handleStateChange(GAME_DEF_STATE_PLAYER_ASSIGN);
            return payload;
        }
        return payload;
    }

    void loop(bool isEnter, const stateCommon::LoopData& data){
        if(isEnter) {
            _error = true;
            _totalEnumerations = 0;
            _enumeration = 0;
            distributedTask::reset();
            buttonSingleClicked();
        }
        distributedTask::loop(data, GAME_DEF_ACTION_ENUMERATE_TASK, enumerateHandler);
        const byte taskState = distributedTask::getState();
        if(taskState == DISTRIBUTED_TASK_STATE_IDLE) {
            animate::pulse(WHITE, 8);
            if(buttonSingleClicked()) {
                distributedTask::init(GAME_DEF_ACTION_ENUMERATE_TASK, enumerateHandler, 0);
            }
            return;
        }
        if(_error) {
            animate::pulse(RED, 4);
            return;
        } 
        animate::spin(WHITE, 4);
    }

    byte getTotalEnumerations(){
        return _totalEnumerations;
    }

    byte getMyEnumeration(){
        return _enumeration;
    }

}