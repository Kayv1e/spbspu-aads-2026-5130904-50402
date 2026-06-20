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

  void getProcessDetails(std::istream& in, std::ostream& out, novikov::SnapshotTree& snaps)
  {
    std::string arg;
    std::string snap_name;
    if (!(in >> arg >> snap_name)) return;

    if (!snaps.has(snap_name))
    {
      out << "<SNAPSHOT NOT FOUND>\n";
      throw std::runtime_error("Snapshot not found");
    }

    Snapshot* snap = snaps.get(snap_name);
    ProcessSnapTree& tree = snap->processTree;

    bool is_pid = !arg.empty();
    for (char c : arg)
    {
      if (!std::isdigit(static_cast< unsigned char >(c)))
      {
        is_pid = false;
        break;
      }
    }

    if (is_pid)
    {
      DWORD target_pid = std::stoul(arg);
      bool found = false;

      for (auto it = tree.begin(); it != tree.end(); ++it)
      {
        for (const auto& proc : (*it).second)
        {
          if (proc.pid == target_pid)
          {
            out << "Process " << target_pid << " details:\n";
            out << "Name: " << toUtf8(proc.name) << "\n";
            out << "Path: " << toUtf8(proc.path) << "\n";
            out << "CPU usage: " << proc.cpuUsage << " %\n";
            out << "RAM usage: " << proc.ramUsage << " MB\n";
            found = true;
            break;
          }
        }
        if (found) break;
      }

      if (!found)
      {
        out << "<PROCESS NOT FOUND>\n";
        throw std::runtime_error("Process not found");
      }
    }
    else
    {
      std::wstring target_name = toWString(arg);

      if (!tree.has(target_name))
      {
        out << "<PROCESS NOT FOUND>\n";
        throw std::runtime_error("Process not found");
      }

      const auto& procs = tree.get(target_name);

      double total_cpu = 0.0;
      size_t total_ram = 0;

      out << arg << " details:\n";
      out << "PIDs: ";
      for (size_t i = 0; i < procs.size(); ++i)
      {
        out << procs[i].pid;
        if (i + 1 < procs.size()) out << ", ";
        total_cpu += procs[i].cpuUsage;
        total_ram += procs[i].ramUsage;
      }
      out << "\n";
      out << "CPU usage: " << total_cpu << " %\n";
      out << "RAM usage: " << total_ram << " MB\n";
    }
  }
}
