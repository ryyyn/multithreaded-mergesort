#include <iostream>
#include <random>
#include <functional>
#include <chrono>
#include <boost/program_options/option.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "utility.hxx"
#include "sort_strategy.hxx"

namespace po = boost::program_options;

int sort_floor = 100;

int main (int argc, char **argv)
{
  try
    {
      // Parse program options
      po::options_description desc("Allowed options");
      desc.add_options()
        ("help", "Produce this help message")
        ("floor,f", po::value<int>(), "Minimum size of array required to use concurrency")
        ("strategy,s", po::value<std::string>(), "Sorting strategy: insertion, gnusort1")
        ("input-file,i", po::value<std::string>(), "Input file")
        ("output-file,o", po::value<std::string>(), "Output file.  Default: input_file_name.sorted")
        ("random,r", po::value<unsigned int>(), "Generate random data (ignores input file)")
        ("generate,g", "Generate data (does not sort). Must supply --random")
        ("verbose,v", "Generate verbose logging")
        ;

      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);

      if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
      }

      std::vector<int> data;

      if (vm.count ("generate") && !vm.count ("random"))
        {
          if (vm.count ("verbose")) std::cerr << "Must provide --random with --generate\n";
          return -1;
        }

      if (vm.count ("random"))
        {
          if (vm.count ("verbose")) std::cout << "Generating random data...\n";
          std::default_random_engine generator (std::chrono::system_clock::now ().time_since_epoch ().count ());
          std::uniform_int_distribution<int> distribution(0,9000);
          data.resize (vm["random"].as < unsigned int > ());
          std::generate (data.begin (),
                         data.end (),
                         std::bind (distribution, generator));
        }
      else
        {
          if (!vm.count ("strategy"))
            {
              std::cerr << "No valid strategy defined!\n";
              return -1;
            }

          if (vm.count ("verbose")) std::cout << "Reading data from file "
                                              <<  vm["input-file"].as < std::string > () << std::endl;
          File<int> file (vm["input-file"].as < std::string > ());
          file.read (data);
          if (vm.count ("verbose")) std::cout << "Read in " << data.size () << "records\n";
        }

      if (!vm.count ("generate"))
        {
          std::unique_ptr<cs281::Sort_Strategy<int> > sort_strategy
            (cs281::Sort_Strategy<int>::get_strategy (vm["strategy"].as < std::string > ()));
        
          // set floor
          if (vm.count("floor"))
            {
              sort_floor = vm["floor"].as<int> ();
            }

          sort_strategy->sort (data);

          if (!std::is_sorted (data.begin (), data.end ()))
            {
              std::cerr << "Error:  Array is not sorted!\n";
            }
        }

      File<int> file (vm.count ("output-file") ? vm["output-file"].as < std::string > () :
                      (vm["input-file"].as < std::string > () + ".sorted"));
      file.write (data);
    }
  catch (std::exception &ex)
    {
      std::cerr << "Caught fatal exception: " << ex.what () << std::endl;
      return -1;
    }

  return 0;
}
