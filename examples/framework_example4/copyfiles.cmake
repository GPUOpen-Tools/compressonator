
macro(cmp_app_copy_to_output source dest)
    cmp_copy_to_output(framework_example4 ${source} ${dest})
#    message(STATUS "Copying: ${source} to ${dest}")
endmacro()

if (CMP_HOST_APPLE)
    # Following: https://developer.apple.com/library/archive/documentation/CoreFoundation/Conceptual/CFBundles/BundleTypes/BundleTypes.html#//apple_ref/doc/uid/10000123i-CH101-SW1
    set(PLUGINS_PATH $<TARGET_BUNDLE_CONTENT_DIR:framework_example4>/plugins)
else()
    set(PLUGINS_PATH $<TARGET_FILE_DIR:framework_example4>/plugins)
endif()


# cmp_app_copy_to_output(plugins/*.dll ${PLUGINS_PATH}/*.dll)
