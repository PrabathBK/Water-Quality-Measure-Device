import 'dart:ffi';

import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:firebase_database/ui/firebase_animated_list.dart';
import 'package:flutter/material.dart';
import 'package:hydrolink_testing/pages/AddDevicePage.dart';

class NewDevicePage extends StatefulWidget {
  @override
  _NewDevicePageState createState() => _NewDevicePageState();
}

class _NewDevicePageState extends State<NewDevicePage> {
  bool buttonClicked = false;
  String _scanResult = '';
  String selectedTankType = ''; // Default selected container
  String selectedType = ''; // Default selected type
  String _selectedCapacity = 'Select Capacity';
  bool _connectButtonClicked = false;
  bool _finalButtonClicked = false;
  String _userTank = '';
  List<String> _tankParameters = [];
  List _tankParametersValues = [];
  String _errorText = '';

  double PH = 7.0;
  double Conductivity = 0;
  double Temperature = 0;
  double Turbidity = 0;
  String Switch = "ON";
  double Battery = 0;
  String Suitability = "OK";

  Query dbRef2 = FirebaseDatabase.instance.ref();
  DatabaseReference barcodeReference =
      FirebaseDatabase.instance.ref().child('RealTimeDB/Barcode');
  DatabaseReference usersReference =
      FirebaseDatabase.instance.ref().child('RealTimeDB/Users');
  DatabaseReference tanksReference =
      FirebaseDatabase.instance.ref().child('RealTimeDB/Tanks');

  // final _customCapacityController = TextEditingController();
  final _nickNameController = TextEditingController();

  late User userFire; // Declare userFire with late keyword

  @override
  void initState() {
    super.initState();
    userFire =
        FirebaseAuth.instance.currentUser!; // Initialize userFire in initState

    String nameUser = userFire.displayName!;
  }

  void signUserOut() {
    FirebaseAuth.instance.signOut();
  }

  void addTankInRealTimeDB(Map database) async {
    String result = await Navigator.push(
      context,
      MaterialPageRoute(builder: (context) => AddDevicePage()),
    );

    // Check if the result is not null and has 8 characters
    if (result != null && result.length == 10) {
      // Perform the necessary operations with the obtained code
      // For now, I'm just printing it. You can update it accordingly.
      print('Obtained Code: $result');
      print(database['Barcodes'][result]);
      setState(() {
        _scanResult = result;
        _userTank = database['Barcodes'][result];
        if (!database['Users'][userFire.uid].containsValue(_userTank)) {
          Map<String, dynamic> updater = {
            'Tank' + ((database['Users'][userFire.uid].length)).toString():
                _userTank,
          };
          usersReference.child(userFire.uid).update(updater);
        }
        if (database['Tanks'][_userTank] == null) {
          Map<String, dynamic> updater = {
            'PH': 0,
            'Turbidity': 0,
            'Conductivity': 25,
            'Temp': 100,
            'switch': 'ON',
            'Battery': 0,
            'Suitable': "Yes",
          };
          tanksReference.child(_userTank).update(updater);
        }
        ;

        //tanksReference.child(_userTank).update(updater);
        if (database['Tanks'][_userTank] != null) {
          PH = database['Tanks'][_userTank]['PH'].toDouble();
          Conductivity =
              database['Tanks'][_userTank]['Conductivity'].toDouble();
          Turbidity = database['Tanks'][_userTank]['Turbidity'].toDouble();
          Switch = database['Tanks'][_userTank]['switch'];
          Battery = database['Tanks'][_userTank]['Battery'].toDouble();
          Suitability = database['Tanks'][_userTank]['Suitable'];
          Temperature = database['Tanks'][_userTank]['Temp'].toDouble();
          _nickNameController.text = database['Tanks'][_userTank]['NickName'];
        }
        //selectedType = database['Tanks'][_userTank][result];
      });

      // Add your logic to update the database or perform other actions.
    }
  }

  void setButtonClicked(bool value) {
    setState(() {
      buttonClicked = value;
    });
  }

  void selectContainer(String type) {
    setState(() {
      //selectedContainer = containerName;
      selectedType = type;
    });
  }

  void selectContainerTankType(String type) {
    setState(() {
      //selectedContainer = containerName;
      selectedTankType = type;
    });
  }

  Widget NewDeviceCharacters({required Map dBItem}) {
    if (_tankParameters.isEmpty && _tankParametersValues.isEmpty) {
      _tankParametersValues = dBItem['Parameters']['Tanks'].keys.toList()
        ..map((item) => item.toString()).toList();

      for (var item in _tankParametersValues) {
        _tankParameters.add(item.toString());
      }
      _tankParameters.add('Select Capacity');
    }
    print('Parameters' + _tankParameters.toString());

    return Column(
      children: [
        Padding(
          padding: EdgeInsets.only(
              top: MediaQuery.of(context).size.height * 0.075,
              bottom: MediaQuery.of(context).size.height * 0.04),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.center,
            children: [
              // Account Information
              Container(
                padding: EdgeInsets.all(16),
                child: Column(
                  children: [
                    // Account Logo
                    Row(
                      children: [
                        Padding(
                          padding: const EdgeInsets.only(right: 16),
                          child: GestureDetector(
                            onTapDown: (_) {
                              setButtonClicked(true);
                            },
                            onTapUp: (_) {
                              setButtonClicked(false);
                            },
                            onTapCancel: () {
                              setButtonClicked(false);
                            },
                            onTap: () {
                              // AuthService().signInwithGoogle();
                              addTankInRealTimeDB(dBItem);
                            },
                            child: Container(
                              decoration: BoxDecoration(
                                color: buttonClicked
                                    ? Theme.of(context)
                                        .colorScheme
                                        .onPrimary
                                        .withOpacity(0.6)
                                    : Theme.of(context)
                                        .colorScheme
                                        .onInverseSurface,
                                borderRadius: BorderRadius.circular(10),
                                boxShadow: buttonClicked
                                    ? [
                                        BoxShadow(
                                          color: Theme.of(context)
                                              .colorScheme
                                              .onInverseSurface,
                                          offset: Offset(3, 3),
                                          blurRadius: 15,
                                          spreadRadius: 1,
                                        ),
                                        BoxShadow(
                                          color: Theme.of(context)
                                              .colorScheme
                                              .onInverseSurface,
                                          offset: Offset(-3, -3),
                                          blurRadius: 15,
                                          spreadRadius: 1,
                                        ),
                                      ]
                                    : [
                                        BoxShadow(
                                          color: Theme.of(context)
                                              .colorScheme
                                              .onSurfaceVariant,
                                          offset: Offset(6, 6),
                                          blurRadius: 15,
                                          spreadRadius: 1,
                                        ),
                                        BoxShadow(
                                          color: Theme.of(context)
                                              .colorScheme
                                              .onError,
                                          offset: Offset(-6, -6),
                                          blurRadius: 15,
                                          spreadRadius: 1,
                                        ),
                                      ],
                              ),
                              child: Padding(
                                padding: const EdgeInsets.all(12),
                                child: Icon(
                                  _scanResult == ''
                                      ? Icons.add_circle_outline
                                      : Icons.check_circle_outline,
                                  //color: Colors.black,
                                  color: _scanResult == ''
                                      ? Colors.red
                                      : Colors.green,
                                  size: 45,
                                ),
                              ),
                            ),
                          ),
                        ),
                        SizedBox(height: 16),
                        // User Name
                        Column(
                          mainAxisAlignment: MainAxisAlignment.center,
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            Text(
                              _scanResult == ''
                                  ? 'No Device Added'
                                  : 'Device ID: $_scanResult',
                              style: TextStyle(
                                fontSize: 18,
                                fontWeight: FontWeight.bold,
                              ),
                            ),
                            SizedBox(height: 8),
                            // User Email
                            Text(
                              'johndoe@example.com',
                              style: TextStyle(
                                fontSize: 14,
                                color: Colors.grey,
                              ),
                            ),
                            SizedBox(height: 8),
                            // Connected Text
                            Text(
                              'Connected',
                              style: TextStyle(
                                fontSize: 14,
                                color: Colors.green,
                              ),
                            ),
                          ],
                        ),
                      ],
                    ),
                  ],
                ),
              ),
              // Change Account Information
              // Change Language
              ListTile(
                leading: Icon(Icons.language),
                title: Text('Set Nickname'),
                onTap: () {
                  // TODO: Implement change language functionality
                },
              ),
              Padding(
                padding:
                    const EdgeInsets.symmetric(horizontal: 10, vertical: 10),
                child: Container(
                  padding: EdgeInsets.symmetric(horizontal: 16.0),
                  decoration: BoxDecoration(
                    border: Border.all(
                        color: Theme.of(context).colorScheme.onSecondary),
                    borderRadius: BorderRadius.circular(8.0),
                  ),
                  child: TextField(
                    controller: _nickNameController,
                    keyboardType: TextInputType.number,
                    decoration: InputDecoration(
                      labelText: 'Nick Name (Optional)',
                      border: InputBorder.none,
                      fillColor: Theme.of(context).colorScheme.tertiary,
                    ),
                  ),
                ),
              ),
              Text(_errorText, style: TextStyle(color: Colors.red)),
              GestureDetector(
                onTapDown: (_) {
                  setState(() {
                    _finalButtonClicked = true;
                  });
                  ;
                },
                onTapUp: (_) {
                  setState(() {
                    _finalButtonClicked = false;
                  });
                },
                onTapCancel: () {
                  setState(() {
                    _finalButtonClicked = false;
                  });
                },
                onTap: () {
                  Map<String, dynamic> updater = {
                    'NickName': _nickNameController.text,
                  };
                  tanksReference.child(_userTank).update(updater);

                  Map<String, String> updater2 = {
                    'Selected Tank': _userTank,
                  };
                  usersReference.child(userFire.uid).update(updater2);
                  Navigator.pop(context);
                },
                child: Container(
                  width: double.infinity,
                  padding: const EdgeInsets.symmetric(vertical: 15.0),
                  margin: EdgeInsets.only(
                    top: 10,
                    bottom: MediaQuery.of(context).size.height * 0.14,
                  ),
                  decoration: BoxDecoration(
                    color: _finalButtonClicked
                        ? Theme.of(context).colorScheme.secondary
                        : Theme.of(context).colorScheme.secondary,
                    borderRadius: BorderRadius.circular(8),
                    boxShadow: _finalButtonClicked
                        ? [
                            BoxShadow(
                              color: Theme.of(context)
                                  .colorScheme
                                  .secondary
                                  .withOpacity(0.3),
                              offset: Offset(3, 3),
                              blurRadius: 15,
                              spreadRadius: 5,
                            ),
                            BoxShadow(
                              color: Theme.of(context)
                                  .colorScheme
                                  .secondary
                                  .withOpacity(0.3),
                              offset: Offset(-3, -3),
                              blurRadius: 15,
                              spreadRadius: 5,
                            ),
                          ]
                        : [
                            BoxShadow(
                              color: Theme.of(context)
                                  .colorScheme
                                  .onSurfaceVariant,
                              offset: Offset(6, 6),
                              blurRadius: 15,
                              spreadRadius: 1,
                            ),
                            BoxShadow(
                              color: Theme.of(context).colorScheme.onSurface,
                              offset: Offset(-6, -6),
                              blurRadius: 15,
                              spreadRadius: 1,
                            ),
                          ],
                  ),
                  child: Center(
                    child: Text(
                      'Submit',
                      style: TextStyle(
                        color: Theme.of(context).colorScheme.background,
                        fontWeight: FontWeight.bold,
                        //fontSize: 16,
                      ),
                    ),
                  ),
                ),
              ),
            ],
          ),
        ),
      ],
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Theme.of(context).colorScheme.primary,
      body: SingleChildScrollView(
        child: Container(
          decoration: BoxDecoration(
            gradient: LinearGradient(
              colors: [
                Theme.of(context).colorScheme.primary,
                Theme.of(context).colorScheme.secondary,
              ],
              begin: Alignment.topCenter,
              end: Alignment.bottomCenter,
            ),
          ),
          child: SizedBox(
            height: MediaQuery.of(context).size.height * 1.3,
            child: FirebaseAnimatedList(
              physics: const NeverScrollableScrollPhysics(),
              query: dbRef2,
              itemBuilder: (BuildContext context, DataSnapshot snapshot,
                  Animation<double> animation, int index) {
                Map? dataBase = snapshot.value as Map?;
                if (dataBase == null) {
                  return SizedBox
                      .shrink(); // Return an empty widget if data is null
                }
                dataBase['key'] = snapshot.key;

                return NewDeviceCharacters(dBItem: dataBase);
              },
            ),
          ),
        ),
      ),
    );
  }

  Widget buildContainer(String type, IconData icon) {
    return GestureDetector(
      onTap: () {
        selectContainer(type);
      },
      child: Container(
        width: 80,
        height: 80,
        decoration: BoxDecoration(
          color: selectedType == type
              ? Theme.of(context).colorScheme.onPrimary.withOpacity(0.6)
              : Theme.of(context).colorScheme.onSurfaceVariant,
          borderRadius: BorderRadius.circular(10),
        ),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Icon(
              icon,
              color: Colors.white,
              size: 30,
            ),
            SizedBox(height: 8),
            Text(
              type,
              style: TextStyle(
                fontSize: 12,
                color: Colors.white,
              ),
            ),
          ],
        ),
      ),
    );
  }

  Widget buildContainerTankType(String type, IconData icon) {
    return GestureDetector(
      onTap: () {
        selectContainerTankType(type);
      },
      child: Container(
        width: 80,
        height: 80,
        decoration: BoxDecoration(
          color: selectedTankType == type
              ? Theme.of(context).colorScheme.onPrimary.withOpacity(0.6)
              : Theme.of(context).colorScheme.onSurfaceVariant,
          borderRadius: BorderRadius.circular(10),
        ),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Icon(
              icon,
              color: Colors.white,
              size: 30,
            ),
            SizedBox(height: 8),
            Text(
              type,
              style: TextStyle(
                fontSize: 12,
                color: Colors.white,
              ),
            ),
          ],
        ),
      ),
    );
  }
}
