install(TARGETS qv2ray RUNTIME DESTINATION bin)
install(FILES ${CMAKE_SOURCE_DIR}/assets/qv2ray.metainfo.xml DESTINATION share/metainfo)
install(FILES ${CMAKE_SOURCE_DIR}/assets/qv2ray.desktop DESTINATION share/applications)
install(FILES ${CMAKE_SOURCE_DIR}/assets/icons/qv2ray.svg DESTINATION share/icons/hicolor/scalable/apps)

set(QV2RAY_ICON_DIMENSIONS 16 22 32 48 64 128 256 512 1024)

foreach(d ${QV2RAY_ICON_DIMENSIONS})
    install(FILES assets/icons/qv2ray.${d}.png
        DESTINATION share/icons/hicolor/${d}x${d}/apps
        RENAME qv2ray.png)
endforeach(d)

if(NOT QV2RAY_EMBED_TRANSLATIONS)
    install(FILES ${QV2RAY_QM_FILES}
        DESTINATION share/qv2ray/lang)
endif()
