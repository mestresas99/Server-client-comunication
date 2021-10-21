struct user_message {
    int result;
    opaque message [256];
    opaque sender [256];
};

program MESSAGE_STORAGE {
    version MESSAGE_STORAGEVER {
        int rpc_register(string username<>)=1;
        int rpc_unregister(string username<>)=2;
        int rpc_user_exists(string username<>)=3;
        int rpc_retrieve_last_identifier(string username<>)=4;
        int rpc_retrieve_message_number(string username<>)=5;
        user_message rpc_retrieve_user_message(string username<>, int message_identifier)=6;
        int store_user_message(string username<>,string username_sender<>, string message<>)=7;
        int delete_user_message(string username<>, int message_identifier)=8;
    } = 1;
} = 99;

