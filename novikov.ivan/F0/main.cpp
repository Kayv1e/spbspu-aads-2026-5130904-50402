#include <iostream>
#include <string>
#include <limits>
#include <map>
#include "AVLTree.hpp"
#include "commands.hpp"
#include "ProcessInfo.hpp"

int main()
{
  std::map< std::string, novikov::ProcessSnapTree > snapshots;

  using cmd_t = void (*)(std::istream& in, std::ostream& out, std::map< std::string, novikov::ProcessSnapTree >& snaps);

  novikov::AVLTree< std::string, cmd_t, std::less< std::string > > cmds;

  cmds["snap"] = novikov::snap;
  cmds["displayAllProcesses"] = novikov::displayAllProcesses;
  cmds["getProcessDetails"] = novikov::getProcessDetails;
  cmds["compareSnapshots"] = novikov::compareSnapshots;
  cmds["filterRAM"] = novikov::filterRAM;
  cmds["filterCPU"] = novikov::filterCPU;
  cmds["exportSnapshot"] = novikov::exportSnapshot;
  cmds["list"] = novikov::list;
  cmds["clearAll"] = novikov::clearAll;
  cmds["help"] = novikov::help;

  std::string cmd;

  while (std::cin >> cmd)
  {
    try
    {
      if (!cmds.has(cmd))
      {
        throw std::logic_error("Invalid command");
      }
      cmds[cmd](std::cin, std::cout, snapshots);
    }
    catch (const std::exception&)
    {
      std::cin.clear();
      auto toIgnore = std::numeric_limits< std::streamsize >::max();
      std::cin.ignore(toIgnore, '\n');
    }
  }

  if (!std::cin.eof())
  {
    std::cerr << "Invalid input\n";
    return 1;
  }

  return 0;
}
