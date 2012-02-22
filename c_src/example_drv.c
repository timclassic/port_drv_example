/* port_driver.c */

#include <stdio.h>
#include "erl_interface.h"
#include "erl_driver.h"
#include "complex.h"

typedef struct {
    ErlDrvPort port;
} example_data;

static ErlDrvData example_drv_start(ErlDrvPort port, char *buff) {
    example_data* d = (example_data*)driver_alloc(sizeof(example_data));
    d->port = port;
    return (ErlDrvData)d;
}

static void example_drv_stop(ErlDrvData handle) {
    driver_free((char*)handle);
}

static void example_drv_output(ErlDrvData handle, char *buff, ErlDrvSizeT bufflen) {
    example_data* d = (example_data*)handle;
    unsigned int res = 0;
    ETERM *tuplep = NULL;
    ETERM *fnp = NULL, *argp = NULL;
    ETERM *intp = NULL;
    unsigned int intp_len = 0;

    erl_init(NULL, 0);
    tuplep = erl_decode(buff);
    fnp = erl_element(1, tuplep);
    argp = erl_element(2, tuplep);

    if (strncmp(ERL_ATOM_PTR(fnp), "foo", 3) == 0) {
        res = foo(ERL_INT_VALUE(argp));
    } else if (strncmp(ERL_ATOM_PTR(fnp), "bar", 3) == 0) {
        res = bar(ERL_INT_VALUE(argp));
    }

    intp = erl_mk_int(res);
    intp_len = erl_encode(intp, buff);
    
    driver_output(d->port, buff, intp_len);
    erl_free_term(tuplep);
    erl_free_term(fnp);
    erl_free_term(argp);
}

ErlDrvEntry example_drv_entry = {
    NULL,		/* F_PTR init, N/A */
    example_drv_start,	/* L_PTR start, called when port is opened */
    example_drv_stop,	/* F_PTR stop, called when port is closed */
    example_drv_output,	/* F_PTR output, called when erlang has sent */
    NULL,		/* F_PTR ready_input, called when input descriptor ready */
    NULL,		/* F_PTR ready_output, called when output descriptor ready */
    "example_drv",	/* char *driver_name, the argument to open_port */
    NULL,		/* F_PTR finish, called when unloaded */
    NULL,		/* handle */
    NULL,		/* F_PTR control, port_command callback */
    NULL,		/* F_PTR timeout, reserved */
    NULL,		/* F_PTR outputv, reserved */
    NULL,		/* Readasync */
    NULL,		/* Flush */
    NULL,
    NULL,
    ERL_DRV_EXTENDED_MARKER,
    ERL_DRV_EXTENDED_MAJOR_VERSION,
    ERL_DRV_EXTENDED_MINOR_VERSION,
    0,
    NULL,
    NULL,
    NULL
};

/* must match name in driver_entry */
DRIVER_INIT(example_drv) {
    return &example_drv_entry;
}
