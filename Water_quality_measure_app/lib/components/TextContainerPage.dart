import 'package:flutter/material.dart';

class TextContainerPage extends StatelessWidget {
  final String text;

  TextContainerPage({required this.text});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Text Container Page'),
      ),
      body: Center(
        child: Text(text),
      ),
    );
  }
}
