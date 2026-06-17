#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include "BSTree.hpp"
#include "Parse.hpp"
#include "commands.hpp"

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "Invalid arguments\n";
    return 1;
  }

  std::ifstream in(argv[1]);
  if (!in.is_open())
  {
    std::cerr << "Could not open the file" << '\n';
    return 1;
  }

  novikov::List< novikov::BSTree< size_t, std::string, std::less< size_t > > > trees;
  using cmd_t = void (*)(std::istream& in, std::ostream& out, BSTList& bstl);
  novikov::BSTree< std::string, cmd_t, std::less< std::string > > cmds;
  cmds["print"] = novikov::print;
  cmds["complement"] = novikov::complement;
  cmds["intersect"] = novikov::intersect;
  cmds["union"] = novikov::union_;

  std::string cmd;
  while (std::cin >> cmd)
  {
    try
    {
      cmds.at(cmd)(std::cout, std::cin, trees);
    }
    catch (...)
    {
      std::cout << "<INVALID COMMAND>" << '\n';
      auto toIgnore = std::numeric_limits< std::streamsize >::max();
      std::cin.ignore(toIgnore, '\n');
    }
  }
  if (!std::cin.eof())
  {
    std::cerr << "Invalid input\n";
    return 1;
  }
}
