import 'package:flutter/material.dart';

class squareElement extends StatefulWidget {
  final String title;
  final String value;

  const squareElement({
    Key? key,
    required this.title,
    required this.value,
  }) : super(key: key);

  @override
  _squareElementState createState() => _squareElementState();
}

class _squareElementState extends State<squareElement> {
  bool isHovered = false;

  @override
  Widget build(BuildContext context) {
    return InkWell(
      onHover: (value) {
        setState(() {
          isHovered = value;
        });
      },
      child: Padding(
        padding: EdgeInsets.symmetric(
            horizontal: MediaQuery.of(context).size.width * 0.09),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Padding(
                padding: EdgeInsets.only(
                    bottom: MediaQuery.of(context).size.height * 0.05)),
            Text(
              widget.title,
              textAlign: TextAlign.center,
              style: TextStyle(
                fontWeight: FontWeight.bold,
                fontSize: 20,
                color: isHovered
                    ? Colors.blue
                    : Color.fromARGB(255, 243, 243, 243),
              ),
            ),
            SizedBox(
              width: MediaQuery.of(context).size.width * 0.3,
              height: MediaQuery.of(context).size.height * 0.1,
              child: Container(
                padding: EdgeInsets.all(16),
                decoration: BoxDecoration(
                  borderRadius: BorderRadius.circular(8),
                  border: Border.all(
                    color: Colors.grey,
                    width: 1,
                  ),
                  color:
                      isHovered ? Colors.blue.withOpacity(0.1) : Colors.white,
                ),
                child: Text(
                  widget.value,
                  style: TextStyle(
                    fontSize: 20,
                    color: isHovered ? Colors.blue : Colors.black,
                  ),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
