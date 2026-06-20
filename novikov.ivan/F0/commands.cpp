#include "commands.hpp"
#include <stdexcept>

namespace novikov
{
  void snap(std::istream& in, std::ostream& out, novikov::SnapshotTree& snaps)
  {
    std::string snap_name;
    if (!(in >> snap_name)) return;

    if (snaps.has(snap_name))
    {
      out << "<SNAPSHOT NAME EXISTS>\n";
      throw std::runtime_error("Snapshot name exists");
    }

    Snapshot* new_snap = new Snapshot();
    new_snap->name = snap_name;
    new_snap->parent = nullptr;

    takeSystemSnapshot(new_snap->processTree);

    snaps.push(snap_name, new_snap);

    out << "Snapshot " << snap_name << " has been created.\n";
  }

  void displayAllProcesses(std::istream& in, std::ostream& out, novikov::SnapshotTree& snaps)
  {
    std::string snap_name;
    if (!(in >> snap_name)) return;

    if (!snaps.has(snap_name))
    {
      out << "<SNAPSHOT NOT FOUND>\n";
      throw std::runtime_error("Snapshot not found");
    }

    Snapshot* snap = snaps.get(snap_name);
    ProcessSnapTree& tree = snap->processTree;

    for (auto it = tree.begin(); it != tree.end(); ++it)
    {
      out << toUtf8((*it).first) << " (PIDs: ";

      const auto& procs = (*it).second;
      for (size_t i = 0; i < procs.size(); ++i)
      {
        out << procs[i].pid;
        if (i + 1 < procs.size())
        {
          out << ", ";
        }
      }
      out << ")\n";
    }
  }
}
