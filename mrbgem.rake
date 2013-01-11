MRuby::Gem::Specification.new('mruby-fltk2') do |spec|
  spec.license = 'MIT'
  spec.authors = 'mattn'

  if ENV['OS'] == 'Windows_NT'
    spec.mruby_libs = '-lfltk2 -lfltk2_images -lws2_32 -lgdi32 -lstdc++ -lole32 -luuid'
  else
    spec.mruby_libs = '-lfltk2 -lfltk2_images'
  end
end
