echo "updating compile commands"
xmake project -k compile_commands
move /Y compile_commands.json .vscode/ 

echo "updating visual studio project"
xmake project -k vsxmake