import lit.formats

config.name = "LibSDCParse Tests"
config.test_format = lit.formats.ShTest(True)

config.suffixes = ['.sdc']

config.test_source_root = os.path.dirname(__file__)
config.test_exec_root = os.path.join(config.my_obj_root, 'test_sdcs')

config.substitutions.append(('%sdcparse-test',
                            os.path.join(config.my_obj_root, 'sdcparse_test')))

config.substitutions.append(('%not',
                            "python3 " + os.path.join(os.path.dirname(__file__), 'not.py')))
