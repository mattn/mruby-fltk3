MRuby::Gem::Specification.new('mruby-fltk3') do |spec|
  spec.license = 'MIT'
  spec.authors = 'mattn'

  spec.cxx.flags << "-std=c++0x -fpermissive #{`fltk3-config --cflags`.delete("\n\r")}"
  if ENV['OS'] == 'Windows_NT'
    spec.linker.flags << "#{`fltk3-config --use-images --ldflags`.delete("\n\r").gsub(/ -mwindows /, ' ')} -lgdi32 -lstdc++".split(" ").reverse
  else
    spec.linker.flags << `fltk3-config --use-images --ldflags`.delete("\n\r").split(" ").reverse
  end
end
