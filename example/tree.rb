#!mruby

window = FLTK3::DoubleWindow.new(360, 300, "tree")
window.begin do
  tree = FLTK3::Tree.new(10, 10, 220, 280)
  tree.root_label = Dir.pwd
  tree.showroot = 1
  add_dir = lambda do |dir, prefix, depth|
    Dir.entries(dir).sort.each do |name|
      next if name.start_with?(".")
      path = prefix.empty? ? name : "#{prefix}/#{name}"
      item = tree.add(path)
      full = "#{dir}/#{name}"
      if File.directory?(full)
        add_dir.call(full, path, depth + 1) if depth < 2
        tree.close(item, false)
      end
    end
  end
  add_dir.call(".", "", 0)

  info = FLTK3::MultilineOutput.new(240, 10, 110, 280)
  info.wrap = true
  tree.callback do |t|
    item = t.item_clicked
    next unless item
    case t.callback_reason
    when FLTK3::TREE_REASON_SELECTED
      info.value = "selected\n#{t.item_pathname(item)}"
    when FLTK3::TREE_REASON_OPENED
      info.value = "opened\n#{item.label}"
    when FLTK3::TREE_REASON_CLOSED
      info.value = "closed\n#{item.label}"
    end
  end
  window.resizable = tree
end
window.show

FLTK3::run
