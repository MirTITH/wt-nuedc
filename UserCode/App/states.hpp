#pragma once

#include <atomic>

extern void StateStop_OnEnter();
extern void StateStop_OnExit();

extern void StateActiveInv_OnEnter();
extern void StateActiveInv_OnExit();

extern void StatePllInv_OnEnter();
extern void StatePllInv_OnExit();

extern void StatePllOnGridInv_OnEnter();
extern void StatePllOnGridInv_OnExit();

enum class AppState_t {
    Stop,        // 停止模式，所有继电器关闭，PWM 关闭
    ActiveInv,   // 主动逆变
    PllInv,      // 锁相逆变
    PllOnGridInv // 并网逆变
};

class AppStateClass
{
private:
    std::atomic<AppState_t> state_ = AppState_t::Stop;

    void ExitState(AppState_t state_to_exit)
    {
        switch (state_to_exit) {
            case AppState_t::Stop:
                StateStop_OnExit();
                break;
            case AppState_t::ActiveInv:
                StateActiveInv_OnExit();
                break;
            case AppState_t::PllInv:
                StatePllInv_OnExit();
                break;
            case AppState_t::PllOnGridInv:
                StatePllOnGridInv_OnExit();
                break;
        }
        state_ = AppState_t::Stop;
    }

    void EnterState(AppState_t state_to_enter)
    {
        switch (state_to_enter) {
            case AppState_t::Stop:
                StateStop_OnEnter();
                break;
            case AppState_t::ActiveInv:
                StateActiveInv_OnEnter();
                break;
            case AppState_t::PllInv:
                StatePllInv_OnEnter();
                break;
            case AppState_t::PllOnGridInv:
                StatePllOnGridInv_OnEnter();
                break;
        }
        state_ = state_to_enter;
    }

public:
    void SwitchTo(AppState_t new_state)
    {
        if (new_state == AppState_t::Stop) {
            ExitState(state_);
            EnterState(new_state);
        } else if (state_ == AppState_t::Stop) {
            ExitState(state_);
            EnterState(new_state);
        }
    }

    AppState_t GetState() const
    {
        return state_;
    }
};

extern AppStateClass kAppState;
