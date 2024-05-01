import 'package:firebase_database/ui/firebase_animated_list.dart';
import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:get/get_connect/http/src/utils/utils.dart';
import 'package:hydrolink_testing/components/percentage.dart';
import 'package:hydrolink_testing/components/percentage2.dart';
import 'package:hydrolink_testing/components/waterflow.dart';
import 'package:hydrolink_testing/pages/AddDevicePage.dart';
import 'package:hydrolink_testing/pages/NewDevicePage.dart';
import 'package:hydrolink_testing/components/squareElement.dart';

class Dashboard extends StatefulWidget {
  final String userUid;
  Dashboard({super.key, required this.userUid});

  @override
  State<Dashboard> createState() => _DashboardState();
}

class _DashboardState extends State<Dashboard> {
  String user = 'HYD00001';

  String _selectedControlType = 'Manual';
  double _percentageValue = 0.0;
  bool _switchValue = false;
  //bool _auto = false;

  Query dbRef2 = FirebaseDatabase.instance.ref();
  DatabaseReference reference =
      FirebaseDatabase.instance.ref().child('RealTimeDB/Tanks');
  DatabaseReference reference2 =
      FirebaseDatabase.instance.ref().child('RealTimeDB/Users');

  final GlobalKey<ScaffoldState> _scaffoldKey = GlobalKey<ScaffoldState>();

  void manSwitchChanged(bool value) {
    Map<String, bool> tanks = {
      'ManSwitch': value,
    };
    reference.child(user).update(tanks).then((value) => null);
    setState(() {
      _switchValue = value;
    });
  }

  void ctrlTypeChanged(String? value) {
    Map<String, bool> tanks = {
      'Auto': value == 'Manual' ? false : true,
    };
    reference.child(user).update(tanks).then((value) => null);
    setState(() {
      _selectedControlType = value!;
    });
  }

  void addTankInRealTimeDB() async {
    String result = await Navigator.push(
      context,
      MaterialPageRoute(builder: (context) => AddDevicePage()),
    );

    // Check if the result is not null and has 8 characters
    if (result != null && result.length == 10) {
      // Perform the necessary operations with the obtained code
      // For now, I'm just printing it. You can update it accordingly.
      print('Obtained Code: $result');

      // Add your logic to update the database or perform other actions.
    }
  }

  void addUserInRealTimeDB(String userUID) {
    Map<String, String> newUserTank = {
      'Tank 01': 'None',
      'Selected Tank': 'None',
    };
    Map<String, Map> newUser = {
      userUID: newUserTank,
    };
    reference2.update(newUser).then((value) => null);
    reference2.child(userUID).update(newUserTank).then((value) => null);
  }

  Widget Dash({required Map tank, required String userUID}) {
    print('PH' + tank['Tanks'][user]['PH'].toString());
    print('conductivity' + tank['Tanks'][user]['Conductivity'].toString());
    print("UserUID: " + userUID);
    print("User Name : " + userUID);
    print("User Data: " + tank['Users'][userUID].toString());
    print('Parameters' + tank['Parameters']['Tanks'].keys.toList().toString());

    if (tank['Users'][userUID] != null) {
      //_switchValue = true;
    } else {
      addUserInRealTimeDB(userUID);
    }
    ;
    return tank['Users'][userUID]['Selected Tank'] == 'None'
        ? Column(
            //if a device doesnt added
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Padding(
                padding: EdgeInsets.only(
                    top: MediaQuery.of(context).size.height * 0.35,
                    bottom: MediaQuery.of(context).size.height * 035),
                child: Column(
                  children: [
                    Text(
                      'No Device Selected. Add one.',
                      style: TextStyle(
                        fontSize: 30,
                        fontWeight: FontWeight.bold,
                        color: Theme.of(context).colorScheme.onPrimary,
                      ),
                    ),
                    ButtonBar(
                      alignment: MainAxisAlignment.center,
                      buttonPadding: EdgeInsets.all(20),
                      children: [
                        ElevatedButton(
                          onPressed: () {
                            Navigator.push(
                                context,
                                MaterialPageRoute(
                                    builder: (context) => NewDevicePage()));
                          },
                          child: Text(
                            'Add Device',
                            style: TextStyle(
                              fontSize: 20,
                              fontWeight: FontWeight.bold,
                              color: Theme.of(context).colorScheme.onPrimary,
                            ),
                          ),
                          style: ElevatedButton.styleFrom(
                            backgroundColor:
                                Theme.of(context).colorScheme.primary,
                            shape: RoundedRectangleBorder(
                              borderRadius: BorderRadius.circular(20),
                            ),
                          ),
                        ),
                      ],
                    ),
                  ],
                ),
              ),
            ],
          )
        : Column(
            //when a device added
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Padding(
                padding: EdgeInsets.only(
                    top: MediaQuery.of(context).size.height * 0.04,
                    bottom: MediaQuery.of(context).size.height * 0.04),
                child: Text(
                  'Water Quality Measurements',
                  style: TextStyle(
                    fontSize: 20,
                    fontWeight: FontWeight.bold,
                    color: Theme.of(context).colorScheme.onPrimary,
                  ),
                ),
              ),
              Row(
                children: [
                  Expanded(
                    child: squareElement(
                      title: 'PH',
                      value: tank['Tanks'][user]['PH'].toString(),
                    ),
                  ),
                  Expanded(
                    child: squareElement(
                      title: 'Conductivity',
                      value: tank['Tanks'][user]['Conductivity'].toString(),
                    ),
                  ),
                ],
              ),
              Row(
                children: [
                  Expanded(
                    child: squareElement(
                      title: 'Turbidity',
                      value: tank['Tanks'][user]['Turbidity'].toString(),
                    ),
                  ),
                  Expanded(
                    child: squareElement(
                      title: 'Temperature',
                      value: tank['Tanks'][user]['Temp'].toString(),
                    ),
                  ),
                ],
              ),
              Row(
                children: [
                  Center(
                    child: Column(
                      mainAxisSize: MainAxisSize
                          .min, // This will size the column to fit its children.
                      children: [
                        Text(
                          'Start',
                          style: TextStyle(
                            fontSize: 16,
                            fontWeight: FontWeight.bold,
                            color: Theme.of(context).colorScheme.onPrimary,
                          ),
                        ),
                        Switch(
                          value: _switchValue,
                          onChanged: (value) {
                            manSwitchChanged(
                                value); // Call the appropriate function to handle the state change
                          },
                        ),
                      ],
                    ),
                  )
                ],
              ),
              Row(
                  //Add text field as output
                  )
            ],
          );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      //key: _scaffoldKey,
      extendBody: true,
      backgroundColor: const Color.fromARGB(255, 183, 206, 245),
      body: Container(
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
        child: Column(
          children: [
            //Text("Logged In: " + userFire.uid!),

            SizedBox(
              height: MediaQuery.of(context).size.height * 0.8795,
              child: FirebaseAnimatedList(
                physics: const NeverScrollableScrollPhysics(),
                query: dbRef2,
                itemBuilder: (BuildContext context, DataSnapshot snapshot,
                    Animation<double> animation, int index) {
                  Map tank = snapshot.value as Map;
                  //mySmartDevices[index][2] =
                  //    student[user][switchParams[index]];
                  tank['key'] = snapshot.key;

                  return Dash(tank: tank, userUID: widget.userUid);
                },
              ),
            ),
          ],
        ),
      ),
    );
  }
}
