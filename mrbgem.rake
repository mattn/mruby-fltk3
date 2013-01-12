MRuby::Gem::Specification.new('mruby-fltk3') do |spec|
  spec.license = 'MIT'
  spec.authors = 'mattn'

  spec.cxxflags = "-fpermissive #{`fltk3-config --cflags`.delete("\n\r")}"
  spec.mruby_libs = `fltk3-config --ldflags`.delete("\n\r")
end
