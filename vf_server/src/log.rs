pub enum InfoType{
    WrongMsgSource,
    FinishAsyncTaskFailed,
}

pub fn get_info_type_str(info_type:InfoType) -> &'static str {
    match info_type {
        InfoType::WrongMsgSource => {
            return "WrongMsgSource";
        }
        InfoType::FinishAsyncTaskFailed => {
            return "FinishAsyncTaskFailed";
        }
    }
}