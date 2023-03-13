// FIXME: remove code duplication (see questions.js)

function HideTickets() {
  var tickets = document.getElementsByClassName('ticket');
  Array.from(tickets).map(function (ticket) { ticket.style.display = 'none'; });

  HideAnswers();
  HideProofs();
  HideDerivations();
  HideSubtitles();
}

function ShowTickets() {
  var tickets = document.getElementsByClassName('ticket');
  Array.from(tickets).map(function (ticket) { ticket.style.display = 'list-item'; });

  ShowAnswers();
  ShowProofs();
  ShowSubtitles();
}

setup('ticket');

