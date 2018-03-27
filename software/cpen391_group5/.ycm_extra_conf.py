import os
import ycm_core

# Python config script for YouCompleteMe
# Also auto generates ColorCoded configs if they don't exist

def DirectoryOfThisScript():
  return os.path.dirname( os.path.abspath( __file__ ) )


def CreateFlags(project_folder, project_bsp_folder):
    flags = [
        '-Wall',
        '-std=c++14',
        '-DALT_NO_INSTRUCTION_EMULATION',
        '-DALT_SINGLE_THREADED',
        '-D__hal__',
        '-I'+project_bsp_folder,
        '-I'+os.path.join(project_bsp_folder,'HAL/inc'),
        '-I'+os.path.join(project_bsp_folder,'drivers/inc'),
        '-I'+os.path.join(project_folder,'libs'),
        '-I'+project_folder,
        '-I'+os.path.join(project_folder, '../shared_libs'),
        '-include',
        os.path.join(project_folder,'.ycm.h'),
        '-isystem',
        os.path.join(project_bsp_folder,'HAL/inc')
    ]

    return flags



def FlagsForFile( filename, **kwargs ):
    relative_to = os.path.join(DirectoryOfThisScript(), '..')

    project_name = os.path.relpath(filename, relative_to).split(os.path.sep)[0]
    if project_name[-4:] == '_bsp':
        project_name = project_name[:-4]

    project_folder = os.path.join(relative_to, project_name)
    project_bsp_folder = os.path.join(relative_to, project_name + '_bsp')
    make_bsp_color_coded = True
    if not os.path.isdir(project_bsp_folder):
        make_bsp_color_coded = False
        project_bsp_folder = os.path.join(relative_to, 'cpen391_group5_bsp')

    flags = CreateFlags(project_folder, project_bsp_folder)

    project_color_coded = os.path.join(project_folder, '.color_coded')
    if not os.path.isfile(project_color_coded):
        with open(project_color_coded, 'w') as f:
            f.write('\n'.join(flags) + '\n')

    bsp_color_coded = os.path.join(project_bsp_folder, '.color_coded')
    if make_bsp_color_coded and not os.path.isfile(bsp_color_coded):
        with open(bsp_color_coded, 'w') as f:
            f.write('\n'.join(flags) + '\n')

    return {
        'flags': flags
    }

