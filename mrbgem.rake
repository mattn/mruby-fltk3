MRuby::Gem::Specification.new('mruby-fltk3') do |spec|
  spec.license = 'MIT'
  spec.authors = 'mattn'

  spec.cxxflags = "-fpermissive #{`fltk3-config --cflags`.delete("\n\r")}"
  if ENV['OS'] == 'Windows_NT'
    spec.mruby_libs = "#{`fltk3-config --ldflags`.delete("\n\r")} -mconsole -lstdc++"
  else
    spec.mruby_libs = `fltk3-config --ldflags`.delete("\n\r")
  end
end
