var randomIndex = 0;
var randomTickets = [];

function Shuffle(array) {
  var copy = array.slice();
  for (let i = copy.length - 1; i > 0; i--) {
    let j = Math.floor(Math.random() * (i + 1));
    var tmp = copy[i];
    copy[i] = copy[j];
    copy[j] = tmp;
  }
  return copy;
}

function HideTickets() {
  var tickets = document.getElementsByClassName("ticket");
  for (var i = 0; i < tickets.length; ++i) {
    tickets[i].style.display = "none";
  }

  var subtitles = document.getElementsByClassName("subtitle");
  for (var i = 0; i < subtitles.length; ++i) {
    subtitles[i].style.display = "none";
  }
}

function ShowTickets() {
  var tickets = document.getElementsByClassName("ticket");
  for (var i = 0; i < tickets.length; ++i) {
    tickets[i].style.display = "block";
  }

  var subtitles = document.getElementsByClassName("subtitle");
  for (var i = 0; i < subtitles.length; ++i) {
    subtitles[i].style.display = "block";
  }
}

function ShowRandomTicket() {
  HideTickets();

  randomTickets[randomIndex].style.display = "none";

  randomIndex += 1;

  if (randomIndex == randomTickets.length) {
    randomTickets = Shuffle(randomTickets).slice();
    randomIndex = 0;
  }

  randomTickets[randomIndex].style.display = "block";
}

window.onload = function() {
  var tickets = Array.from(document.getElementsByClassName("ticket"));
  randomTickets = Shuffle(tickets).slice();
};

