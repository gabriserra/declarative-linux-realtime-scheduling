
void rts_request_add(struct rts_request_list* l, struct rts_request* req) {
    list_ptr_add_top(&(l->reqs), (void*) req);
}

void rts_req_set_message(struct rts_msg_req* req, int msg_type) {
    req->msg_type = msg_type;
}