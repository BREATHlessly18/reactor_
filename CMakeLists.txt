add_subdirectory(core)

set(
    HEADERS 
    core/connector.hh
    core/copyability.hh
    core/epoller.hh
    core/i_object.hh
    core/server.hh
    core/threadpool.hh
    core/timer.hh
    core/loopthread.hh
)

# 自行指定安装路径
install(FILES ${HEADERS} DESTINATION ${PROJECT_SOURCE_DIR}/install)

