
link_directories(${CONAN_LIBS} "${CMAKE_BINARY_DIR}/lib")

link_libraries(${CONAN_LIBS})

add_executable("test_${PROJECT_NAME}"
    "test_${PROJECT_NAME}.cpp"
    )

