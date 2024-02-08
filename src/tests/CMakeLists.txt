list(APPEND SRCS ${CMAKE_CURRENT_LIST_DIR}/TestClearColor.cpp 
                 ${CMAKE_CURRENT_LIST_DIR}/TestMultipleObjects.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/TestTriangle.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/TestUniform.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/TestBatchTexture.cpp)

set(SRCS ${SRCS} PARENT_SCOPE)
