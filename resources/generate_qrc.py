import os

def generate_qrc(directory, output_file):
    with open(output_file, 'w') as qrc:
        qrc.write('<!DOCTYPE RCC><RCC version="1.0">\n')
        qrc.write('<qresource>\n')
        for root, dirs, files in os.walk(directory):
            for file in files:
                if file.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp', '.xobj3d')):
                    path = os.path.join(root, file)
                    # Generate a relative path to use as the alias in the .qrc file
                    alias = os.path.relpath(path, directory).replace('\\', '/')
                    # Write the relative path as the file reference in the .qrc file
                    qrc.write(f'    <file alias="{alias}">{alias}</file>\n')
        qrc.write('</qresource>\n')
        qrc.write('</RCC>\n')

# Replace 'your_directory' with the path to your image directory
# Replace 'resources.qrc' with the desired output .qrc file name
generate_qrc('interiorequipmentobjects', 'assets.qrc')
