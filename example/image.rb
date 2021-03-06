#!mruby

window = FLTK3::DoubleWindow.new(100, 100, 400, 430, "mruby-fltk3")
window.begin do
  widget = FLTK3::Widget.new(10, 10, 380, 400)
  button = FLTK3::Button.new(10, 390, 80, 30, "load file")
  button.callback do
    fn = FLTK3::file_chooser("image file", "*.{bm,bmp,gif,jpg,pbm,pgm,png,ppm,xbm,xpm}")
    image = fn ? FLTK3::SharedImage::get(fn) : nil
    if image
      if image.w > widget.w || image.h > widget.h
        if image.w > image.h
          temp = image.copy(widget.w, widget.h * image.h / image.w)
        else
          temp = image.copy(widget.w * image.w / image.h, widget.h)
        end
        image.release
        image = temp
      end
    end
    widget.image = image
    widget.redraw
  end
  window.resizable = widget
end
window.show

FLTK3::run

