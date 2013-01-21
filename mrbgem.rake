MRuby::Gem::Specification.new('mruby-fltk3') do |spec|
  spec.license = 'MIT'
  spec.authors = 'mattn'

  spec.cxx.flags = "-std=c++0x -fpermissive #{`fltk3-config --cflags`.delete("\n\r")}"
  if ENV['OS'] == 'Windows_NT'
    spec.linker.flags << "#{`fltk3-config --use-images --ldflags`.delete("\n\r").gsub(/ -mwindows /, ' ')} -lgdi32 -lstdc++"
  else
    spec.linker.flags << `fltk3-config --use-images --ldflags`.delete("\n\r")
  end
end
