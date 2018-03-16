import os
import ycm_core

# Python config script for YouCompleteMe
# Special linux kernel version
# Also auto generates ColorCoded configs if they don't exist

def DirectoryOfThisScript():
  return os.path.dirname( os.path.abspath( __file__ ) )


def CreateFlags(kern_abs_path):
    flags = [
        '-Wall',
        '-Werror',
        '-std=gnu89',
        '-Wmissing-prototypes',
        '-Wstrict-prototypes',
        '-fomit-frame-pointer',
        '-I'+os.path.join(kern_abs_path, 'linux-4.5/include'),
    ]

    return flags



def FlagsForFile( filename, **kwargs ):
    kern_folder = DirectoryOfThisScript()

    flags = CreateFlags(kern_folder)

    color_coded = os.path.join(kern_folder, '.color_coded')
    if not os.path.isfile(color_coded):
        with open(color_coded, 'w') as f:
            f.write('\n'.join(flags) + '\n')

    return {
        'flags': flags
    }

