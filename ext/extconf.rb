require "mkmf"

have_library('IL', 'ilInit')
extension_name = "ris"
dir_config(extension_name)
create_makefile(extension_name)
