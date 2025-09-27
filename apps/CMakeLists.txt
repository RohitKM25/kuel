add_executable(kc app.cpp)
target_compile_features(kc PRIVATE cxx_std_17)

target_link_libraries(kc PRIVATE kuel_library)
