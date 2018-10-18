#include "rts_types.h"

enum QUERY_TYPE {
    RTS_BUDGET,
    RTS_REMAINING_BUDGET
};

enum REQ_TYPE {
    RTS_CAP_QUERY,
    RTS_RSV_CREATE,
    RTS_RSV_ATTACH,
    RTS_RSV_DETACH,
    RTS_RSV_QUERY,
    RTS_RSV_DESTROY
};

enum REP_TYPE {
    RTS_CAP_QUERY_REPLY,
    RTS_CAP_QUERY_INVALID,
    RTS_RSV_CREATED,
    RTS_RSV_UNAVAILABLE,
    RTS_RSV_NOT_CREATED,
    RTS_RSV_ATTACHED,
    RTS_RSV_ATTACH_INVALID,
    RTS_RSV_QUERY_REPLY,
    RTS_RSV_QUERY_INVALID,
    RTS_RSV_DESTROY_OK
};

struct rts_request {
    enum REQ_TYPE req_type;
    union {
        struct rts_ids ids;
        struct rts_params param;
        enum QUERY_TYPE query_type;
    } payload;
};

struct rts_reply {
    enum REP_TYPE rep_type;
    float payload;
};
