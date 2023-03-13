// FIXME: remove code duplication (see questions.js)

function HideTasks() {
  var tasks = document.getElementsByClassName('task');
  Array.from(tasks).map(function (task) { task.style.display = 'none'; });

  HideAnswers();
  HideProofs();
  HideDerivations();
  HideSubtitles();
}

function ShowTasks() {
  var tasks = document.getElementsByClassName('task');
  Array.from(tasks).map(function (task) { task.style.display = 'list-item'; });

  ShowAnswers();
  ShowProofs();
  ShowSubtitles();
}

setup('task');

