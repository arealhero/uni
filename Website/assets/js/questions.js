// FIXME: remove code duplication

var randomIndex = 0;
var randomQuestions = [];

var scrollingSpeed = 200;

function handleKeyDown(event) {
  const { ctrlKey, key } = event;
  switch (key) {
    case 'd':
      {
	if (ctrlKey)
	{
	  window.scrollBy({
	    top: 0.5 * window.innerHeight,
	    left: 0,
	  });

	  event.preventDefault();
	}
      }
      break;
    case 'u':
      {
	if (ctrlKey)
	{
	  window.scrollBy({
	    top: -0.5 * window.innerHeight,
	    left: 0,
	  });

	  event.preventDefault();
	}
      }
      break;
    case 'j':
      {
	window.scrollBy({
	  top: 0.5 * scrollingSpeed,
	  left: 0,
	});
      }
      break;
    case 'k':
      {
	window.scrollBy({
	  top: -0.5 * scrollingSpeed,
	  left: 0,
	});
      }
      break;
    default:
      break;
  }

}

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

function Show(content, button) {
  button.textContent = "Скрыть ответ";
  content.style.display = "block";
}

function Hide(content, button) {
  button.textContent = "Показать ответ";
  content.style.display = "none";
}

function ToggleContentVisibility() {
  var button = this.querySelector(".toggle-button");
  var content = this.querySelector(".content");

  if (content.style.display === "none") {
    Show(content, button);
  } else {
    Hide(content, button);
  }
}

function HideProof(proof, button) {
  button.textContent = "Показать доказательство";
  proof.style.display = "none";
}

function ShowProof(proof, button) {
  button.textContent = "Скрыть доказательство";
  proof.style.display = "block";
}

function HideDerivation(derivation, button) {
  button.textContent = "Показать вывод";
  derivation.style.display = "none";
}

function ShowDerivation(derivation, button) {
  button.textContent = "Скрыть вывод";
  derivation.style.display = "block";
}

function ToggleProofVisibility() {
  var button = this.querySelector(".proof-toggle");
  var proof = this.querySelector(".proof");

  if (proof.style.display === "none") {
    ShowProof(proof, button);
  } else {
    HideProof(proof, button);
  }
}

function ToggleDerivationVisibility() {
  var button = this;
  var derivation = this.nextElementSibling;

  if (derivation.style.display === "none") {
    ShowDerivation(derivation, button);
  } else {
    HideDerivation(derivation, button);
  }
}

function HideProofs() {
  var proofButtons = document.getElementsByClassName("proof-toggle");
  for (var i = 0; i < proofButtons.length; ++i) {
    var button = proofButtons[i];
    var buttonParent = button.parentNode;
    var proof = buttonParent.querySelector(".proof");
    HideProof(proof, button);
  }
}
function ShowProofs() {
  var proofButtons = document.getElementsByClassName("proof-toggle");
  for (var i = 0; i < proofButtons.length; ++i) {
    var button = proofButtons[i];
    var buttonParent = button.parentNode;
    var proof = buttonParent.querySelector(".proof");
    ShowProof(proof, button);
  }
}

function HideDerivations() {
  var derivationButtons = document.getElementsByClassName("derivation-toggle");
  for (var i = 0; i < derivationButtons.length; ++i) {
    var button = derivationButtons[i];
    var derivation = button.nextElementSibling;
    HideDerivation(derivation, button);
  }
}

function SetProofButtonsHandlers() {
  var proofButtons = document.getElementsByClassName("proof-toggle");
  for (var i = 0; i < proofButtons.length; ++i) {
    var button = proofButtons[i];
    var buttonParent = button.parentNode;
    button.textContent = "Скрыть доказательство";
    button.onclick = ToggleProofVisibility.bind(buttonParent);
  }
}

function SetDerivationButtonsHandlers() {
  var derivationButtons = document.getElementsByClassName("derivation-toggle");
  for (var i = 0; i < derivationButtons.length; ++i) {
    var button = derivationButtons[i];
    button.textContent = "Показать вывод";
    button.onclick = ToggleDerivationVisibility.bind(button);
  }
}

function HideQuestions() {
  var questions = document.getElementsByClassName("question");
  for (var i = 0; i < questions.length; ++i) {
    var question = questions[i];
    var button = question.querySelector(".toggle-button");
    var content = question.querySelector(".content");

    question.style.display = "none";
    Hide(content, button);
  }

  HideProofs();
  HideDerivations();
}

function ShowQuestions() {
  var questions = document.getElementsByClassName("question");
  for (var i = 0; i < questions.length; ++i) {
    var question = questions[i];
    var button = question.querySelector(".toggle-button");
    var content = question.querySelector(".content");

    question.style.display = "block";
    Show(content, button);
  }

  ShowProofs();
}

function ShowRandomQuestion() {
  if (randomQuestions.length === 0) return;

  HideQuestions();

  randomQuestions[randomIndex].style.display = "none";

  randomIndex += 1;

  if (randomIndex == randomQuestions.length) {
    randomQuestions = Shuffle(randomQuestions).slice();
    randomIndex = 0;
  }

  randomQuestions[randomIndex].style.display = "block";
}

function HideAnswers() {
  var questions = document.getElementsByClassName("question");
  for (var i = 0; i < questions.length; ++i) {
    var question = questions[i];
    var button = question.querySelector(".toggle-button");
    var content = question.querySelector(".content");

    Hide(content, button);
  }
}

function ShowAnswers() {
  var questions = document.getElementsByClassName("question");
  for (var i = 0; i < questions.length; ++i) {
    var question = questions[i];
    var button = question.querySelector(".toggle-button");
    var content = question.querySelector(".content");

    Show(content, button);
  }
}

window.onload = function() {
  var questions = document.getElementsByClassName("question");
  for (var i = 0; i < questions.length; ++i) {
    var question = questions[i];
    var button = question.querySelector(".toggle-button");
    button.textContent = "Скрыть ответ";
    button.onclick = ToggleContentVisibility.bind(question);
  }

  SetProofButtonsHandlers();

  SetDerivationButtonsHandlers();
  HideDerivations();

  var nonEmptyQuestions = Array.from(questions).filter((question) => {
    var name = question.querySelector(".name");
    return name.textContent.trim() !== "";
  });

  randomQuestions = Shuffle(nonEmptyQuestions).slice();
  document.addEventListener('keydown', handleKeyDown);
};



