#!mruby

window = FLTK3::DoubleWindow.new(420, 320, "chart")
window.begin do
  chart = FLTK3::Chart.new(10, 10, 400, 200, "cpu")
  chart.type = FLTK3::LINE_CHART
  chart.bounds(0, 100)
  chart.maxsize = 40
  chart.color = FLTK3::WHITE

  choice = FLTK3::Choice.new(80, 220, 150, 25, "type")
  %w(BAR_CHART HORBAR_CHART LINE_CHART FILL_CHART SPIKE_CHART PIE_CHART).each do |name|
    choice.add(name)
  end
  choice.value = 2
  choice.callback do
    chart.type = FLTK3.const_get(choice.text)
    chart.redraw
  end

  clock = FLTK3::RoundClock.new(250, 220, 90, 90)

  positioner = FLTK3::Positioner.new(80, 255, 150, 55, "pos")
  positioner.xbounds(0, 100)
  positioner.ybounds(0, 100)

  value = 50
  tick = nil
  tick = Proc.new do
    value = [[value + rand(21) - 10, 0].max, 100].min
    chart.add(value, nil, FLTK3::BLUE)
    FLTK3::repeat_timeout(0.5, &tick)
  end
  FLTK3::add_timeout(0.5, &tick)
end
window.show

FLTK3::run
