//configuração do firebase

    // Import the functions you need from the SDKs you need
    import { initializeApp } from "https://www.gstatic.com/firebasejs/10.13.0/firebase-app.js";
    import { getAnalytics } from "https://www.gstatic.com/firebasejs/10.13.0/firebase-analytics.js";
    // TODO: Add SDKs for Firebase products that you want to use
    // https://firebase.google.com/docs/web/setup#available-libraries

    // Your web app's Firebase configuration
    // For Firebase JS SDK v7.20.0 and later, measurementId is optional
    const firebaseConfig = {
      apiKey: "AIzaSyAf0ubkUu9w0gJxshU7Lu78LGFlN3_QjcA",
      authDomain: "inventario-c50f8.firebaseapp.com",
      databaseURL: "https://inventario-c50f8-default-rtdb.firebaseio.com/rfids",
      projectId: "inventario-c50f8",
      storageBucket: "inventario-c50f8.appspot.com",
      messagingSenderId: "369500930345",
      appId: "1:369500930345:web:aee12cb852f5a8711d7738",
      measurementId: "G-5GJP3TWZPC"
    };

    // Initialize Firebase
    const app = initializeApp(firebaseConfig);
    const analytics = getAnalytics(app);

    import { getDatabase, ref, child, onValue, get, set, update, remove } from "https://www.gstatic.com/firebasejs/10.13.0/firebase-database.js";

    const db = getDatabase();

    //pegar todos os dados da tabela

    let inventario = [];
    let num = 0;
    let tbody = document.getElementById('tbody1');

    const SelectAllDataOnce = () => {
      const dbRef = ref(db);
      //rfids é o nome da categoria no firebase
      get(child(dbRef, 'rfids')).then((snapshot) => {
        inventario = [];
        snapshot.forEach(iid => {
          inventario.push(iid.val());
        });
        AddAllRecords();
      })
    }

    const SelectAllDataRealtime = () => {
      const dbRef = ref(db, 'rfids');
      onValue(dbRef, (snapshot) => {
        inventario = [];
        snapshot.forEach(iid => {
          inventario.push(iid.val());
        });
        AddAllRecords();
      })

    }

    const AddSingleRecord = (Local, Nome, Tag, Patrimonio) => {
      let trow = document.createElement('tr');
      let td1 = document.createElement('td');
      let td2 = document.createElement('td');
      let td3 = document.createElement('td');
      let td4 = document.createElement('td');
      let td5 = document.createElement('td');
      let td6 = document.createElement('td');

      td1.innerHTML = ++num;
      td2.innerHTML = Local;
      td3.innerHTML = Nome;
      td4.innerHTML = Tag;
      td5.innerHTML = Patrimonio;

      let EditButton = document.createElement('button');
      let DelButton = document.createElement('button');

      EditButton.id = 'edit-' + num;
      EditButton.className = 'btn btn-primary me-2';
      EditButton.innerHTML = '<i class="fa-solid fa-file-pen"></i>'
      EditButton.setAttribute("data-bs-toggle", 'modal');
      EditButton.setAttribute("data-bs-target", '#actionModal');
      EditButton.addEventListener('click', LoadModal);

      DelButton.id = 'del-' + num;
      DelButton.className = 'btn btn-danger me-2';
      DelButton.innerHTML = '<i class="fa-solid fa-trash"></i>'
      DelButton.setAttribute("data-bs-toggle", 'modal');
      DelButton.setAttribute("data-bs-target", '#actionModal');
      DelButton.addEventListener('click', LoadModal);

      td6.append(EditButton, DelButton);

      trow.append(td1, td2, td3, td4, td5, td6);
      tbody.append(trow);

    }

    const AddAllRecords = () => {
      num = 0;
      tbody.innerHTML = "";
      inventario.forEach(iid => {
        AddSingleRecord(iid.Local, iid.Nome, iid.Tag, iid.Patrimonio);
      })
    }

    let modXButton = document.getElementById('modXButton');
    let actionLabel = document.getElementById('actionLabel');
    let actionBtn = document.getElementById('actionBtn');
    let modLocal = document.getElementById('modLocal');
    let modNome = document.getElementById('modNome');
    let modTag = document.getElementById('modTag');
    let modPatrimonio = document.getElementById('modPatrimonio');
    let addBtn = document.getElementById('add-0');

    const LoadModal = (event) => {
      var targetId = (event.target.id.length > 1) ? event.target.id : event.target.parentElement.id;

      let string = targetId.split('-');
      let mode = string[0];
      let selectIndex = string[1];

      actionBtn.disabled = true;

      if (mode === 'add') {
        actionBtn.className = 'btn btn-lg btn-sucess';
        actionLabel.innerText = 'Adcionar novo registro';
        actionBtn.innerText = 'Adcionar';
        actionBtn.addEventListener('click', AddData);

        modLocal.value = "";
        modNome.value = "";
        modTag.value = "";
        modPatrimonio.value = "";

        modLocal.disabled = false;
        modNome.disabled = false;
        modTag.disabled = false;
        modPatrimonio.disabled = false;

      }
      else if (mode === 'edit') {
        actionBtn.className = 'btn btn-lg btn-sucess';
        actionLabel.innerText = 'Editar registro';
        actionBtn.innerText = 'Atualizar';
        actionBtn.addEventListener('click', UpdData);

        modLocal.value = rfids[selectIndex].Local;
        modNome.value = rfids[selectIndex].Nome;
        modTag.value = rfids[selectIndex].Tag;
        modPatrimonio.value = rfids[selectIndex].Patrimonio;

        modLocal.disabled = false;
        modNome.disabled = false;
        modTag.disabled = false;
        modPatrimonio.disabled = false;

      }
      else if (mode === 'del') {
        actionBtn.className = 'btn btn-lg btn-danger';
        actionLabel.innerText = 'excluir registro';
        actionBtn.innerText = 'excluir';
        actionBtn.addEventListener('click', DelData);

        modLocal.value = rfids[selectIndex].Local;
        modNome.value = rfids[selectIndex].Nome;
        modTag.value = rfids[selectIndex].Tag;
        modPatrimonio.value = rfids[selectIndex].Patrimonio;

        modLocal.disabled = true;
        modNome.disabled = true;
        modTag.disabled = true;
        modPatrimonio.disabled = true;

      }
    }

    const AddData = () => {
      action.disabled = true;
      set(ref(db, 'rfids/' + modLocal.value), //deve ta errado isso aqui
        {
          Local: modLocal.value,
          Nome: modNome.value,
          Tag: modTag.value,
          Patrimonio: modPatrimonio.value,
        }).then(() => { modXButton.click(); })
    }

    const UpdateData = () => {
      action.disabled = true;
      set(ref(db, 'rfids/' + modLocal.value),
        {
          Local: modLocal.value,
          Nome: modNome.value,
          Tag: modTag.value,
          Patrimonio: modPatrimonio.value,
        }).then(() => { modXButton.click(); })
    }

    addBtn.addEventListener('click, LoadModal');
    window.addEventListener('load', SelectAllDataRealtime);
