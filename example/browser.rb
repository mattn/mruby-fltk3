#!mruby

window = FLTK3::DoubleWindow.new(400, 300, "browser")
window.begin do
  browser = FLTK3::HoldBrowser.new(10, 10, 380, 200)
  browser.column_widths = [120, 80, 0]
  browser.column_char = "\t"
  browser.add "@B12@bName\tSize\tType"
  Dir.entries(".").sort.each do |name|
    next if name.start_with?(".")
    if File.directory?(name)
      browser.add "#{name}\t-\tdirectory"
    else
      browser.add "#{name}\t#{File.size(name)}\tfile"
    end
  end

  output = FLTK3::Output.new(10, 220, 380, 25)
  browser.callback do
    output.value = browser.value > 1 ? browser.text.split("\t").first : ""
  end

  check = FLTK3::CheckBrowser.new(10, 255, 380, 35)
  check.add("bold header", true)
  check.callback do
    browser.text(1, check.checked?(1) ? "@B12@bName\tSize\tType" : "Name\tSize\tType")
    browser.redraw
  end
  window.resizable = browser
end
window.show

FLTK3::run
