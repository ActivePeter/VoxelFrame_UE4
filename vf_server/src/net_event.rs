trait NetEvent{
    fn on_client_disconnected();
    fn on_client_connected();
}
// impl NetEvent for