#include <aji/core/args.h>

AjiDistriArgs *
AjiDistriArgs_Distribute(AjiDistriArgs *dargs, int argc, char **argv) {
    AjiCStrVec *app_args = AjiCStrVec_New();
    AjiCStrVec *cmd_args = AjiCStrVec_New();

    int m = 0;
    for (int i = 0; i < argc; ++i) {
        const char *arg = argv[i];
        switch (m) {
        case 0:
            AjiCStrVec_Push(app_args, arg);
            m = 10;
            break;
        case 10:
            if (arg[0] == '-') {
                AjiCStrVec_Push(app_args, arg);
            } else {
                AjiCStrVec_Push(cmd_args, arg);
                m = 20;
            }
            break;
        case 20:
            AjiCStrVec_Push(cmd_args, arg);
            break;
        }
    }

    dargs->argc = AjiCStrVec_Len(app_args);
    dargs->argv = AjiCStrVec_EscDel(app_args);
    dargs->cmd_argc = AjiCStrVec_Len(cmd_args);
    dargs->cmd_argv = AjiCStrVec_EscDel(cmd_args);

    return dargs;
}
