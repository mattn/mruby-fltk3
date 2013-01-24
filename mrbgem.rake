MRuby::Gem::Specification.new('mruby-fltk3') do |spec|
  spec.license = 'MIT'
  spec.authors = 'mattn'

  spec.cxx.flags << "-std=c++0x -fpermissive #{`fltk3-config --cflags`.delete("\n\r")}"
  if ENV['OS'] == 'Windows_NT'
    fltk3_libs = "#{`fltk3-config --use-images --ldflags`.delete("\n\r").gsub(/ -mwindows /, ' ')} -lgdi32 -lstdc++".split(" ")
  else
    fltk3_libs = "#{`fltk3-config --use-images --ldflags`.delete("\n\r")} -lstdc++".split(" ")
  end
  flags = fltk3_libs.reject {|e| e =~ /^-l/ }
  libraries = fltk3_libs.select {|e| e =~ /-l/ }.map {|e| e[2..-1] }
  spec.linker.flags << flags
  spec.linker.libraries << libraries
end
