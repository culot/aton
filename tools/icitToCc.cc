#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <regex>
#include <vector>
#include <string>

void writeHeader(std::ofstream& out) {
  out << "#include <string>" << std::endl;
  out << "#include <iostream>" << std::endl;
  out << "#include <unistd.h>" << std::endl;
  out << std::endl;
  out << "#include <zmq.h>" << std::endl;
  out << std::endl;
  out << "#include \"common.h\"" << std::endl;
  out << std::endl;
  out << std::endl;
  out << "int main(void) {" << std::endl;
  out << "  std::cout << \"Connecting to server... \";" << std::endl;
  out << "  void* context = zmq_ctx_new();" << std::endl;
  out << "  void* requester = zmq_socket(context, ZMQ_REQ);" << std::endl;
  out << "  zmq_connect(requester, \"tcp://localhost:5555\");" << std::endl;
  out << "  std::cout << \"OK\" << std::endl;" << std::endl;
  out << std::endl;
  out << "  common::clearServerMemory(requester);" << std::endl;
  out << std::endl;
}

void writeFooter(std::ofstream& out) {
  out << "  zmq_close(requester);" << std::endl;
  out << "  zmq_ctx_destroy(context);" << std::endl;
  out << "  return 0;" << std::endl;
  out << "}" << std::endl;
}

bool isWellFormed(const std::string& line) {
  if (line.empty() || line.size() < 2) {
    std::cerr << "Line too short (should contain at least '++') ["
              << line << "]" << std::endl;
    return false;
  }

  if (line.front() != '+' || line.back() != '+') {
    std::cerr << "Line should begin and end with '+' ["
              << line << "]" << std::endl;
    return false;
  }

  return true;
}

std::vector<std::string> tokenize(const std::string& line) {
  std::regex re("-");
  std::sregex_token_iterator first{line.begin(), line.end(), re, -1};
  std::sregex_token_iterator last;

  return {first, last};
}

void processFiles(std::ofstream& out, std::vector<std::string>& ifnames) {
  for (const auto& f : ifnames) {
    std::ifstream in(f);
    if (!in) {
      std::cerr << "Could not open [" << f << "] for reading" << std::endl;
      continue;
    }
    std::string line;
    while (std::getline(in, line)) {
      if (!isWellFormed(line)) {
        std::cerr << "Malformed line [" << line << "] - skipping" << std::endl;
        continue;
      }
      line = line.substr(1, line.size() - 2); // Remove enclosing '+' signs
      std::vector<std::string> tokens = tokenize(line);
      for (const auto& token : tokens) {
        out << "  common::send(requester, \"" << token << "\");" << std::endl;
      }

      out << std::endl;
      out << "  common::sendJuncture(requester);" << std::endl;
      out << std::endl;
    }
    in.close();
  }
}

int main(int argc, char** argv) {
  const std::string ofname("icitToCc.out");
  std::ofstream out;
  out.open(ofname, std::ios::trunc);
  if (!out.is_open()) {
    std::cerr << "Could not open output file [" << ofname << "]" << std::endl;
    return 1;
  }

  if (argc <= 1) {
    std::cerr << "Missing input file name(s)" << std::endl;
    return 1;
  }

  std::vector<std::string> ifnames;
  for (int i = 1; i < argc; ++i) {
    std::string ifname(argv[i]);
    ifnames.push_back(ifname);
  }

  std::cout << "Starting conversion for ["
            << ifnames.size() << "] input files" << std::endl;

  writeHeader(out);
  processFiles(out, ifnames);
  writeFooter(out);

  out.close();

  return 0;
}
