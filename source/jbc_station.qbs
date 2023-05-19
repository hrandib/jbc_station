import qbs
import qbs.FileInfo

Project {
    name: "jbc_station"
    references: [
        "chibios-qbs/chibios.qbs",
        "lvgl-qbs/lvgl.qbs",
    ]

    property path CH_PATH: sourceDirectory + "/ChibiOS/"
    property path LV_PATH: sourceDirectory + "/lvgl/"
    property string CORE: "cortex-m4"
    property string MCU: "STM32F401xE"

    Product {
        name: "config"

        Depends { name: "cpp" }

        Export {
            Depends { name: "cpp" }

            cpp.driverFlags: [
                "-mcpu=" + CORE,
                "--specs=nano.specs",
                "-mfloat-abi=hard",
                "-mfpu=fpv4-sp-d16",
            ]

            cpp.commonCompilerFlags: [
                "-fdata-sections",
                "-ffunction-sections",
                "-flto=auto", "-ffat-lto-objects",
                "-ffast-math",
                "-ffp-contract=fast",
                "-fsingle-precision-constant",
                "-Wdouble-promotion",
            ]

            cpp.cxxFlags: [
                "-Wno-register",
                "-Wno-volatile",
                "-Wno-deprecated-enum-enum-conversion",
                "-fconcepts-diagnostics-depth=2"
            ]

            cpp.cLanguageVersion: "gnu17"
            cpp.cxxLanguageVersion: "gnu++23"

            cpp.includePaths: [
                "config",
                "board"
            ]

            cpp.defines: [
                MCU,
                "CH_CUSTOMER_LIC_PORT_CM4=TRUE",
                "CORTEX_USE_FPU"
            ]

            cpp.linkerFlags: [
                "--gc-sections",
                "--defsym=__process_stack_size__=0x800",
                "--defsym=__main_stack_size__=0x800",
                "--no-warn-rwx-segments",
            ]

            cpp.positionIndependentCode: false
            cpp.enableExceptions: false
            cpp.enableRtti: false

            Group {
                name: "Config"
                prefix: "config/"
                files: [
                    "halconf.h",
                    "mcuconf.h",
                    "chconf.h",
                    "lv_conf.h",
                ]
            }

            Properties {
                condition: qbs.buildVariant === "release"
                cpp.debugInformation: false
                cpp.optimization: "small"
            }

            Properties {
                condition: qbs.buildVariant !== "release"
                cpp.debugInformation: true
                cpp.generateLinkerMapFile: true
                cpp.commonCompilerFlags: [ "-Og" ]
            }
            cpp.generateLinkerMapFile: true
        }
    }

    CppApplication {
        name: "jbc_station"
        type: ["printsize"]

        Depends { name: "chibios" }
        Depends { name: "license" }
        Depends { name: "config" }
        Depends { name: "lvgl" }

        consoleApplication: false
        cpp.executableSuffix: ".elf"

        cpp.includePaths: [
            "impl",
            "drivers",
            "utility",
            "resource",
        ]

        Group {
            name: "Compiled object file"
            fileTagsFilter: "application"
            qbs.install: true
        }

        Group {
            name: "Board"
            prefix: "board/"
            files: [
                "board.h",
                "board.c"
            ]
        }

        Group {
            name: "impl"
            prefix: "impl/"
            files: [
                "display_handler.cpp",
                "display_handler.h",
                "main.cpp",
                "sensor_handler.cpp",
                "sensor_handler.h",
            ]
        }

        Group {
            name: "drivers"
            prefix: "drivers/"
            files: [
                "ch_port.h",
                "driver_utils.h",
                "gpio.h",
                "pinlist.h",
                "s1d157xx.h",
                "shiftreg.h",
            ]
        }

        Group {
            name: "utility"
            prefix: "utility/"
            files: [
                "ch_extended.h",
                "chlog.h",
                "cppstreams.h",
            ]
        }

        Group {
            name: "fonts"
            files: [
                "resource/monofonts.h",
                "resource/monofonts.cpp",
                "resource/hooge_mono_50px.c",
                "SSD1306Ascii/src/fonts/*.h", //TODO: include only required
            ]
            cpp.includePaths: outer.concat([
                "SSD1306Ascii/src/fonts"
            ])
        }

        Group {
            name: "various"
            prefix: project.CH_PATH + "/os/various/"
            files: [
                "syscalls.c"
            ]
        }

        Group {
            name: "Linker files"
            prefix: cpp.libraryPaths + "/"
            fileTags: ["linkerscript"]
            files: [ project.MCU + ".ld"]
        }

        Rule {
                id: size
                inputs: ["application"]
                Artifact {
                    fileTags: ["printsize"]
                    filePath: "-"
                }
                prepare: {
                    function getBinutilsSize(arr) {
                        for (var i=0; i < arr.length; ++i) {
                            if ('binutilsPathPrefix' in arr[i]) {
                                return arr[i].binutilsPathPrefix + 'size';
                            }
                        }
                    }
                    var sizePath = getBinutilsSize(product.dependencies)
                    var args = [input.filePath]
                    var cmd = new Command(sizePath, args)
                    cmd.description = "File size: " + FileInfo.fileName(input.filePath)
                    cmd.highlight = "linker"
                    return cmd;
                }
            }

    } //CppApplication

} //Project
