# When linking against vnl-algo, one must also link netlib:
USE_NETLIB := 1

IULIBS += -lvnl-algo
include ${IUEROOT}/core/vnl/config.mk
