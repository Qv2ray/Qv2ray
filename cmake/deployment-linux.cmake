install(TARGETS qv2ray RUNTIME DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/assets/qv2ray.desktop DESTINATION share/applications)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/qv2ray.svg DESTINATION share/icons/hicolor/scalable/apps)

if(BUILD_SHARED_LIBS)
    if(QV2RAY_QV2RAYBASE_PROVIDER STREQUAL "module")
        install(TARGETS Qv2rayBase)
    endif()
endif()

foreach(d 16 22 32 48 64 128 256 512 1024)
    install(FILES assets/icons/qv2ray.${d}.png
        DESTINATION share/icons/hicolor/${d}x${d}/apps
        RENAME qv2ray.png)
endforeach(d)

if(NOT QV2RAY_EMBED_TRANSLATIONS)
    install(FILES ${QV2RAY_QM_FILES}
        DESTINATION share/qv2ray/lang)
endif()
