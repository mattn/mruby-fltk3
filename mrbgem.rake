MRuby::Gem::Specification.new('mruby-fltk3') do |spec|
  spec.license = 'MIT'
  spec.authors = 'mattn'

  spec.cxxflags = "-std=gnu++11 -fpermissive #{`fltk3-config --cflags`.delete("\n\r")}"
  if ENV['OS'] == 'Windows_NT'
    spec.mruby_libs = "#{`fltk3-config --use-images --ldflags`.delete("\n\r").gsub(/ -mwindows /, ' ')} -lgdi32 -lstdc++"
  else
    spec.mruby_libs = `fltk3-config --use-images --ldflags`.delete("\n\r")
  end
end
